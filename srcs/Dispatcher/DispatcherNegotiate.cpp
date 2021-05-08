#include "package.hpp"

void			Dispatcher::negotiate(Client &client)
{
    std::multimap<double, std::string> 	        languageMap;
    std::multimap<double, std::string> 	        charsetMap;

    int             tmp_fd = -1;
    double          tmp_q = 0;
    std::string     tmp_path;
    std::string     tmp_ext;

    int				fd = -1;
    std::string		path;
    std::string		ext;

    if (client.req.headers.find("Accept-Language") != client.req.headers.end())
        _parser.parseAccept(client, languageMap, "Accept-Language");
    if (client.req.headers.find("Accept-Charset") != client.req.headers.end())
        _parser.parseAccept(client, charsetMap, "Accept-Charset");
    if (!languageMap.empty())
    {
        for (std::multimap<double, std::string>::reverse_iterator it(languageMap.rbegin()); it != languageMap.rend(); ++it)
        {
            if (!charsetMap.empty())
            {
                for (std::multimap<double, std::string>::reverse_iterator it2(charsetMap.rbegin()); it2 != charsetMap.rend(); ++it2)
                {
                    tmp_ext = it->second + "." + it2->second;
                    tmp_path = client.conf["savedpath"] + "." + tmp_ext;
                    tmp_fd = open(tmp_path.c_str(), O_RDONLY);
                    if (tmp_fd != -1)
                    {
                        if (it->first * it2->first > tmp_q)
                        {
                            client.res.headers["Content-Language"] = it->second;
                            tmp_q =it->first * it2->first;
                            if (fd != -1)
                                close(fd);
                            fd = tmp_fd;
                            tmp_fd = -1;
                            path = tmp_path;
                            ext = tmp_ext;
                            break ;
                        }
                        else
                            close(tmp_fd);
                    }
                    tmp_ext = it2->second + "." + it->second;
                    tmp_path = client.conf["savedpath"] + "." + tmp_ext;
                    tmp_fd = open(tmp_path.c_str(), O_RDONLY);
                    if (tmp_fd != -1)
                    {
                        if (it->first * it2->first > tmp_q)
                        {
                            client.res.headers["Content-Language"] = it->second;
                            tmp_q = it->first * it2->first;
                            if (fd != -1)
                                close(fd);
                            fd = tmp_fd;
                            tmp_fd = -1;
                            path = tmp_path;
                            ext = tmp_ext;
                            break;
                        }
                        else
                            close(tmp_fd);
                    }
                }
            }
            tmp_ext = it->second;
            tmp_path = client.conf["savedpath"] + "." + tmp_ext;
            tmp_fd = open(tmp_path.c_str(), O_RDONLY);
            if (tmp_fd != -1)
            {
                if (it->first > tmp_q)
                {
                    client.res.headers["Content-Language"] = it->second;
                    tmp_q = it->first;
                    if (fd != -1)
                                close(fd);
                    fd = tmp_fd;
                    tmp_fd = -1;
                    path = tmp_path;
                    ext = tmp_ext;
                    break;
                }
                else
                    close(tmp_fd);
            }
        }
    }
    if (languageMap.empty())
    {
        if (!charsetMap.empty())
        {
            for (std::multimap<double, std::string>::reverse_iterator it2(charsetMap.rbegin()); it2 != charsetMap.rend(); ++it2)
            {
                tmp_ext = it2->second;
                tmp_path = client.conf["savedpath"] + "." + it2->second;
                tmp_fd = open(tmp_path.c_str(), O_RDONLY);
                if (tmp_fd != -1)
                {
                    if (it2->first > tmp_q)
                    {
                        tmp_q = it2->first;
                        if (fd != -1)
                                close(fd);
                        fd = tmp_fd;
                        tmp_fd = -1;
                        path = tmp_path;
                        ext = tmp_ext;
                        break ;
                    }
                    else
                        close(tmp_fd);
                }
            }
        }
    }
    if (fd != -1)
    {
        client.conf["path"] = path;
        client.res.headers["Content-Location"] = client.req.uri + "." + ext;
        if (client.read_fd != -1)
            close(client.read_fd);
        client.read_fd = fd;
        client.res.status_code = OK;
    }
}
