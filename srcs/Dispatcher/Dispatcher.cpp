#include "package.hpp"

Dispatcher* Dispatcher::instance = NULL;

Dispatcher::Dispatcher()
{
    method["GET"] = &Dispatcher::GETHEADMethod;
    method["HEAD"] = &Dispatcher::GETHEADMethod;
    method["PUT"] = &Dispatcher::PUTMethod;
    method["POST"] = &Dispatcher::POSTMethod;
    method["CONNECT"] = &Dispatcher::CONNECTMethod;
    method["TRACE"] = &Dispatcher::TRACEMethod;
    method["OPTIONS"] = &Dispatcher::OPTIONSMethod;
    method["DELETE"] = &Dispatcher::DELETEMethod;
    method["BAD"] = &Dispatcher::handlingBadRequest;

    status["GET"] = &Dispatcher::GETHEADStatus;
    status["HEAD"] = &Dispatcher::GETHEADStatus;
    status["PUT"] = &Dispatcher::PUTStatus;
    status["POST"] = &Dispatcher::POSTStatus;
    status["CONNECT"] = &Dispatcher::CONNECTStatus;
    status["TRACE"] = &Dispatcher::TRACEStatus;
    status["OPTIONS"] = &Dispatcher::OPTIONSStatus;
    status["DELETE"] = &Dispatcher::DELETEStatus;

    MIMETypes[".txt"] = "text/plain";
    MIMETypes[".bin"] = "application/octet-stream";
    MIMETypes[".jpeg"] = "image/jpeg";
    MIMETypes[".jpg"] = "image/jpeg";
    MIMETypes[".html"] = "text/html";
    MIMETypes[".htm"] = "text/html";
    MIMETypes[".png"] = "image/png";
    MIMETypes[".bmp"] = "image/bmp";
    MIMETypes[".pdf"] = "application/pdf";
    MIMETypes[".tar"] = "application/x-tar";
    MIMETypes[".json"] = "application/json";
    MIMETypes[".css"] = "text/css";
    MIMETypes[".js"] = "application/javascript";
    MIMETypes[".mp3"] = "audio/mpeg";
    MIMETypes[".avi"] = "video/x-msvideo";
}

void    Dispatcher::execute(Client &client)
{
    (this->*method[client.req.method])(client);
}

void	Dispatcher::GETHEADMethod(Client &client)
{
    struct stat	file_info;

    switch (client.status)
    {
        case Client::CODE:
            setStatusCode(client);
            fstat(client.read_fd, &file_info);
            if (S_ISDIR(file_info.st_mode) && client.conf["listing"] == "on")
                createListing(client);
            if (client.res.status_code == NOTFOUND)
                negotiate(client);
            if (checkCGI(client) && client.res.status_code == OK)
            {
                executeCGI(client);
                client.status = Client::CGI;
            }
            else
                client.status = Client::HEADERS;
            client.setFileToRead(true);
            break ;
        case Client::CGI:
            if (client.read_fd == -1)
            {
                _parser.parseCGIResult(client);
                client.status = Client::HEADERS;
            }
            break ;
        case Client::HEADERS:
            lstat(client.conf["path"].c_str(), &file_info);
            if (!S_ISDIR(file_info.st_mode))
                client.res.headers["Last-Modified"] = getLastModified(client.conf["path"]);
            if (client.res.headers["Content-Type"][0] == '\0')
                client.res.headers["Content-Type"] = findType(client);
            if (client.res.status_code == UNAUTHORIZED)
                client.res.headers["WWW-Authenticate"] = "Basic";
            else if (client.res.status_code == NOTALLOWED)
                client.res.headers["Allow"] = client.conf["methods"];
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break;
    }

}

void	Dispatcher::POSTMethod(Client &client)
{
    switch (client.status)
    {
        case Client::BODYPARSING:
            _parser.parseBody(client);
            break ;
        case Client::CODE:
            setStatusCode(client);
            if (checkCGI(client) && client.res.status_code == OK)
            {
                executeCGI(client);
                client.status = Client::CGI;
                client.setFileToRead(true);
            }
            else
            {
                if (client.res.status_code == OK || client.res.status_code == CREATED)
                    client.setFileToWrite(true);
                else
                    client.setFileToRead(true);
                client.status = Client::HEADERS;
            }
            break ;
        case Client::CGI:
            if (client.read_fd == -1)
            {
                _parser.parseCGIResult(client);
                client.status = Client::HEADERS;
            }
            break ;
        case Client::HEADERS:
            if (client.res.status_code == UNAUTHORIZED)
                client.res.headers["WWW-Authenticate"] = "Basic";
            else if (client.res.status_code == NOTALLOWED)
                client.res.headers["Allow"] = client.conf["methods"];
            else if (client.res.status_code == CREATED)
                client.res.body = "File created\n";
            else if (client.res.status_code == OK && !checkCGI(client))
                client.res.body = "File modified\n";
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1 && client.write_fd == -1)
            {
                if (client.res.headers["Content-Length"][0] == '\0')
                    client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }
}

void	Dispatcher::PUTMethod(Client &client)
{
    std::string		path;
    std::string		body;

    switch (client.status)
    {
        case Client::BODYPARSING:
            _parser.parseBody(client);
            break ;
        case Client::CODE:
            if (setStatusCode(client))
                client.setFileToWrite(true);
            else
                client.setFileToRead(true);
            if (client.res.status_code == CREATED || client.res.status_code == NOCONTENT)
            {
                client.res.headers["Location"] = client.req.uri;
                if (client.res.status_code == CREATED)
                    client.res.body = "Resource created\n";
            }
            else if (client.res.status_code == NOTALLOWED)
                client.res.headers["Allow"] = client.conf["methods"];
            else if (client.res.status_code == UNAUTHORIZED)
                client.res.headers["WWW-Authenticate"] = "Basic";
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.write_fd == -1 && client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }

}


void	Dispatcher::CONNECTMethod(Client &client)
{
    switch (client.status)
    {
        case Client::CODE:
            setStatusCode(client);
            client.setFileToRead(true);
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }
}

void	Dispatcher::TRACEMethod(Client &client)
{
    switch (client.status)
    {
        case Client::CODE:
            setStatusCode(client);
            if (client.res.status_code == OK)
            {
                client.res.headers["Content-Type"] = "message/http";
                client.res.body = client.req.method + " " + client.req.uri + " " + client.req.version + "\r\n";
                for (std::map<std::string, std::string>::iterator it(client.req.headers.begin());
                     it != client.req.headers.end(); ++it)
                {
                    client.res.body += it->first + ": " + it->second + "\r\n";
                }
            }
            else
                client.setFileToRead(true);
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }
}

void	Dispatcher::OPTIONSMethod(Client &client)
{
    switch (client.status)
    {
        case Client::CODE:
            setStatusCode(client);
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            if (client.req.uri != "*")
                client.res.headers["Allow"] = client.conf["methods"];
            createResponse(client);
            client.status = Client::RESPONSE;
            break ;
    }
}

void	Dispatcher::DELETEMethod(Client &client)
{
    switch (client.status)
    {
        case Client::CODE:
            if (!setStatusCode(client))
                client.setFileToRead(true);
            if (client.res.status_code == OK)
            {
                unlink(client.conf["path"].c_str());
                client.res.body = "File deleted\n";
            }
            else if (client.res.status_code == NOTALLOWED)
                client.res.headers["Allow"] = client.conf["methods"];
            else if (client.res.status_code == UNAUTHORIZED)
                client.res.headers["WWW-Authenticate"] = "Basic";
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }
}

void	Dispatcher::handlingBadRequest(Client &client)
{
    switch (client.status)
    {
        case Client::CODE:
            client.res.version = "HTTP/1.1";
            client.res.status_code = BADREQUEST;
            getErrorPage(client);
            client.setFileToRead(true);
            client.res.headers["Date"] = ft::getDate();
            client.res.headers["Server"] = "webserv";
            client.status = Client::BODY;
            break ;
        case Client::BODY:
            if (client.read_fd == -1)
            {
                client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
                createResponse(client);
                client.status = Client::RESPONSE;
            }
            break ;
    }
}
