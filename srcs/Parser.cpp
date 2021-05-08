#include "package.hpp"

Parser::Parser()
{

}

Parser::~Parser()
{

}

bool			Parser::checkSyntax(const Request &req)
{
	if (req.method.size() == 0 || req.uri.size() == 0
		|| req.version.size() == 0)
		return (false);
	if (req.method != "GET" && req.method != "POST"
		&& req.method != "HEAD" && req.method != "PUT"
		&& req.method != "CONNECT" && req.method != "TRACE"
		&& req.method != "OPTIONS" && req.method != "DELETE")
		return (false);
	if (req.method != "OPTIONS" && req.uri[0] != '/')
		return (false);
	if (req.version != "HTTP/1.1\r" && req.version != "HTTP/1.1")
		return (false);
	if (req.headers.find("Host") == req.headers.end())
		return (false);
	return (true);
}

void			Parser::getClientConf(Client &client, Request &req, std::vector<config> &conf)
{
    std::map<std::string, std::string> elmt;
    std::string		tmp;
    std::string		word;
    struct stat		info;
    struct stat		info_tmp;
    config			to_parse;

    if (!req.valid)
    {
        client.conf["error"] = conf[0]["server|"]["error"];
        return ;
    }
    std::vector<config>::iterator it(conf.begin());
    while (it != conf.end())
    {
        if (req.headers["Host"] == (*it)["server|"]["server_name"])
        {
            to_parse = *it;
            break ;
        }
        ++it;
    }
    if (it == conf.end())
        to_parse = conf[0];
    tmp = req.uri;
    do
    {
        if (to_parse.find("server|location " + tmp + "|") != to_parse.end())
        {
            elmt = to_parse["server|location " + tmp + "|"];
            break ;
        }
        tmp = tmp.substr(0, tmp.find_last_of('/'));
    } while (tmp != "");
    if (elmt.size() == 0)
        if (to_parse.find("server|location /|") != to_parse.end())
            elmt = to_parse["server|location /|"];
    if (elmt.size() > 0)
    {
        client.conf = elmt;
        client.conf["path"] = req.uri.substr(0, req.uri.find("?"));
        if (elmt.find("root") != elmt.end())
        	client.conf["path"].replace(0, tmp.size(), elmt["root"]);
    }
    for (std::map<std::string, std::string>::iterator it(to_parse["server|"].begin()); it != to_parse["server|"].end(); ++it)
    {
        if (client.conf.find(it->first) == client.conf.end())
            client.conf[it->first] = it->second;
    }
    lstat(client.conf["path"].c_str(), &info);
    if (S_ISDIR(info.st_mode))
    {
        if (client.conf["index"][0] && client.conf["listing"] != "on")
        {
            tmp = elmt["index"];
            if (client.conf["index"][0] && client.conf["listing"] != "on")
            {
                while (!tmp.empty())
                {
                    ft::getline(tmp, word, ' ');
                    if (!lstat((client.conf["path"] + "/" + word).c_str(), &info_tmp))
                    {
                        client.conf["path"] +=  "/" + word;
                        break;
                    }
                }
            }
        }
    }
    if (req.method == "GET" || req.method == "HEAD")
        client.conf["savedpath"] = client.conf["path"];
    g_logger.log("path requested from " + client.ip + ":" + std::to_string(client.port) + ": " + client.conf["path"], MED);
}

void			Parser::parseRequest(Client &client, std::vector<config> &conf)
{
    Request				request;
    std::string			tmp;
    std::string			buffer;

    buffer = std::string(client.rBuf);
    if (buffer[0] == '\r')
        buffer.erase(buffer.begin());
    if (buffer[0] == '\n')
        buffer.erase(buffer.begin());
    ft::getline(buffer, request.method, ' ');
    ft::getline(buffer, request.uri, ' ');
    ft::getline(buffer, request.version);
    if (parseHeaders(buffer, request))
        request.valid = checkSyntax(request);
    if (request.uri != "*" || request.method != "OPTIONS")
        getClientConf(client, request, conf);
	client.status = Client::CODE;
	if (request.valid)
	{
		if (client.conf["root"][0] != '\0')
			chdir(client.conf["root"].c_str());
		if (request.method == "POST" || request.method == "PUT")
			client.status = Client::BODYPARSING;
	}
	else
		request.method = "BAD";
    client.req = request;
    tmp = client.rBuf;
    tmp = tmp.substr(tmp.find("\r\n\r\n") + 4);
    strcpy(client.rBuf, tmp.c_str());
}

bool			Parser::parseHeaders(std::string &buf, Request &req)
{
    size_t		pos;
    std::string	line;
    std::string	key;
    std::string	value;

    while (!buf.empty())
    {
        ft::getline(buf, line);
        if (line.size() < 1 || line[0] == '\n' || line[0] == '\r')
            break ;
        if (line.find(':') != std::string::npos)
        {
            pos = line.find(':');
            key = line.substr(0, pos);
            if (line[pos + 1] == ' ')
                value = line.substr(pos + 2);
            else
                value = line.substr(pos + 1);
            if (ft::isspace(value[0]) || ft::isspace(key[0]) || value.empty() || key.empty())
            {
                req.valid = false;
                return (false);
            }
            req.headers[key] = value;
            req.headers[key].pop_back();
        }
        else
        {
            req.valid = false;
            return (false);
        }
    }
    return (true);
}

void			Parser::getBody(Client &client)
{
	unsigned int	bytes;

	if (client.chunk.len == 0)
		client.chunk.len = atoi(client.req.headers["Content-Length"].c_str());
	if (client.chunk.len < 0)
	{
		client.req.method = "BAD";
		client.status = Client::CODE;
		return ;
	}
	bytes = strlen(client.rBuf);
	if (bytes >= client.chunk.len)
	{
		memset(client.rBuf + client.chunk.len, 0, BUFFER_SIZE - client.chunk.len);
		client.req.body += client.rBuf;
		client.chunk.len = 0;
		client.status = Client::CODE;
	}
	else
	{
		client.chunk.len -= bytes;
		client.req.body += client.rBuf;
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
	}
}

int				Parser::findLen(Client &client)
{
	std::string		to_convert;
	int				len;
	std::string		tmp;

	to_convert = client.rBuf;
	to_convert = to_convert.substr(0, to_convert.find("\r\n"));
	while (to_convert[0] == '\n')
		to_convert.erase(to_convert.begin());
	len = fromHexa(to_convert.c_str());
	tmp = client.rBuf;
	tmp = tmp.substr(tmp.find("\r\n") + 2);
	strcpy(client.rBuf, tmp.c_str());
	return (len);
}

int				Parser::fromHexa(const char *nb)
{
	char	base[17] = "0123456789abcdef";
	char	base2[17] = "0123456789ABCDEF";
	int		result = 0;
	int		i;
	int		index;

	i = 0;
	while (nb[i])
	{
		int j = 0;
		while (base[j])
		{
			if (nb[i] == base[j])
			{
				index = j;
				break ;
			}
			j++;
		}
		if (j == 16)
		{
			j = 0;
			while (base2[j])
			{
				if (nb[i] == base2[j])
				{
					index = j;
					break ;
				}
				j++;
			}
		}
		result += index * ft::getpower(16, (strlen(nb) - 1) - i);
		i++;
	}
	return (result);
}

void			Parser::fillBody(Client &client)
{
	std::string		tmp;

	tmp = client.rBuf;
	if (tmp.size() > client.chunk.len)
	{
		client.req.body += tmp.substr(0, client.chunk.len);
		tmp = tmp.substr(client.chunk.len + 1);
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
		strcpy(client.rBuf, tmp.c_str());
		client.chunk.len = 0;
		client.chunk.found = false;
	}
	else
	{
		client.req.body += tmp;
		client.chunk.len -= tmp.size();
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
	}
}

void			Parser::dechunkBody(Client &client)
{
	if (strstr(client.rBuf, "\r\n") && client.chunk.found == false)
	{
		client.chunk.len = findLen(client);
		if (client.chunk.len == 0)
			client.chunk.done = true;
		else
			client.chunk.found = true;
	}
	else if (client.chunk.found == true)
		fillBody(client);
	if (client.chunk.done)
	{
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
		client.status = Client::CODE;
		client.chunk.found = false;
		client.chunk.done = false;
		return ;
	}
}

void			Parser::parseBody(Client &client)
{
    if (client.req.headers.find("Content-Length") != client.req.headers.end())
        getBody(client);
    else if (client.req.headers["Transfer-Encoding"] == "chunked")
        dechunkBody(client);
	else
	{
		client.req.method = "BAD";
		client.status = Client::CODE;
		g_logger.log("body size parsed from " + client.ip + ":" + std::to_string(client.port) + ": " + std::to_string(client.req.body.size()), MED);
	}
}

void			Parser::parseAccept(Client &client, std::multimap<double, std::string> &map, std::string Accept)
{
    std::string							language;
    std::string							to_parse;
    std::string					tmp;
    std::stringstream           tmp2;
    double                              q;

    to_parse = client.req.headers[Accept];
    int i = 0;
    while (to_parse[i] != '\0')
    {
        language.clear();
        tmp2.clear();
        tmp.clear();
        q = 0;
        while (to_parse[i] && to_parse[i] != ',' && to_parse[i] != ';')
            language += to_parse[i++];
        if (to_parse[i] == ',' || to_parse[i] == '\0')
            tmp = "1";
        else if (to_parse[i] == ';')
        {
            i += 3;
            while (to_parse[i] && to_parse[i] != ',')
                tmp += to_parse[i++];
        }
        tmp2 << tmp;
        tmp2 >> q;
        while (to_parse[i] == ',' || to_parse[i] == ' ')
            i++;
        std::pair<double, std::string>	pair(q, language);
        map.insert(pair);
    }
}

void		Parser::parseCGIResult(Client &client)
{
    size_t			pos;
    std::string		headers;
    std::string		key;
    std::string		value;

    if (client.res.body.find("\r\n\r\n") == std::string::npos)
        return ;
    headers = client.res.body.substr(0, client.res.body.find("\r\n\r\n") + 1);
    pos = headers.find("Status");
    if (pos != std::string::npos)
    {
        client.res.status_code.clear();
        pos += 8;
        while (headers[pos] != '\r')
        {
            client.res.status_code += headers[pos];
            pos++;
        }
    }
    pos = 0;
    while (headers[pos])
    {
        while (headers[pos] && headers[pos] != ':')
        {
            key += headers[pos];
            ++pos;
        }
        ++pos;
        while (headers[pos] && headers[pos] != '\r')
        {
            value += headers[pos];
            ++pos;
        }
        client.res.headers[key] = value;
        key.clear();
        value.clear();
        if (headers[pos] == '\r')
			pos++;
		if (headers[pos] == '\n')
			pos++;
    }
    pos = client.res.body.find("\r\n\r\n") + 4;
    client.res.body = client.res.body.substr(pos);
    client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
}
