#include "package.hpp"

static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
                                   56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
                                   7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
                                   0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                                   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

void			Dispatcher::createListing(Client &client)
{
    DIR				*dir;
    struct dirent	*cur;

    close(client.read_fd);
    client.read_fd = -1;
    dir = opendir(client.conf["path"].c_str());
    client.res.body = "<html>\n<body>\n";
    client.res.body += "<h1>Directory listing</h1>\n";
    while ((cur = readdir(dir)) != NULL)
    {
        if (cur->d_name[0] != '.')
        {
            client.res.body += "<a href=\"" + client.req.uri;
            if (client.req.uri != "/")
                client.res.body += "/";
            client.res.body += cur->d_name;
            client.res.body += "\">";
            client.res.body += cur->d_name;
            client.res.body += "</a><br>\n";
        }
    }
    closedir(dir);
    client.res.body += "</body>\n</html>\n";
}

void		    Dispatcher::createResponse(Client &client)
{
    std::map<std::string, std::string>::const_iterator b;

    client.response = client.res.version + " " + client.res.status_code + "\r\n";
    b = client.res.headers.begin();
    while (b != client.res.headers.end())
    {
        if (b->second != "")
            client.response += b->first + ": " + b->second + "\r\n";
        ++b;
    }
    client.response += "\r\n";
    if (client.req.method != "HEAD")
        client.response += client.res.body;
    client.res.clear();
}

std::string		Dispatcher::decode64(const char *data)
{
    unsigned int len = strlen(data);
    unsigned char* p = (unsigned char*)data;
    int pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const size_t L = ((len + 3) / 4 - pad) * 4;
    std::string str(L / 4 * 3 + pad, '\0');
    for (size_t i = 0, j = 0; i < L; i += 4)
    {
        int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
        str[j++] = n >> 16;
        str[j++] = n >> 8 & 0xFF;
        str[j++] = n & 0xFF;
    }
    if (pad)
    {
        int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
        str[str.size() - 1] = n >> 16;

        if (len > L + 2 && p[L + 2] != '=')
        {
            n |= B64index[p[L + 2]] << 6;
            str.push_back(n >> 8 & 0xFF);
        }
    }
    if (str.back() == 0)
        str.pop_back();
    return (str);
}

std::string		Dispatcher::findType(Client &client)
{
    std::string 	extension;
    size_t			pos;

    if (client.conf["path"].find_last_of('.') != std::string::npos)
    {
        pos = client.conf["path"].find('.');
        extension = client.conf["path"].substr(pos, client.conf["path"].find('.', pos + 1) - pos);
        if (MIMETypes.find(extension) != MIMETypes.end())
            return (MIMETypes[extension]);
        else
            return (MIMETypes[".bin"]);
    }
    return ("");
}

void			Dispatcher::getErrorPage(Client &client)
{
    std::string		path;

    path = client.conf["error"] + "/" + client.res.status_code.substr(0, 3) + ".html";
    client.conf["path"] = path;
    client.read_fd = open(path.c_str(), O_RDONLY);
}

std::string		Dispatcher::getLastModified(std::string path)
{
    char		buf[BUFFER_SIZE + 1];
    int			ret;
    struct tm	*tm;
    struct stat	file_info;

    if (lstat(path.c_str(), &file_info) == -1)
        return ("");
    tm = localtime(&file_info.st_mtime);
    ret = strftime(buf, BUFFER_SIZE, "%a, %d %b %Y %T %Z", tm);
    buf[ret] = '\0';
    return (buf);
}

bool            Dispatcher::checkCGI(Client &client)
{
    if (client.conf.find("CGI") != client.conf.end() && client.req.uri.find(client.conf["CGI"]) != std::string::npos)
            return true;
    else if (client.conf.find("php") != client.conf.end() && client.req.uri.find(".php") != std::string::npos)
            return true;
    return false;
}
