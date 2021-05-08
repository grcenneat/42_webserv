#include "package.hpp"

char		**Dispatcher::setCGIEnv(Client &client)
{
    char											**env;
    std::map<std::string, std::string> 				envMap;
    size_t											pos;

    envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
    envMap["SERVER_SOFTWARE"] = "webserv";
    envMap["REQUEST_URI"] = client.req.uri;
    envMap["REQUEST_METHOD"] = client.req.method;
    envMap["REMOTE_ADDR"] = client.ip;
    envMap["PATH_INFO"] = client.req.uri;
    envMap["PATH_TRANSLATED"] = client.conf["path"];
    envMap["CONTENT_LENGTH"] = std::to_string(client.req.body.size());

    if (client.req.uri.find('?') != std::string::npos)
        envMap["QUERY_STRING"] = client.req.uri.substr(client.req.uri.find('?') + 1);
    else
        envMap["QUERY_STRING"];
    if (client.req.headers.find("Content-Type") != client.req.headers.end())
        envMap["CONTENT_TYPE"] = client.req.headers["Content-Type"];
    if (client.conf.find("exec") != client.conf.end())
        envMap["SCRIPT_NAME"] = client.conf["exec"];
    else
        envMap["SCRIPT_NAME"] = client.conf["path"];
    if (client.conf["listen"].find(":") != std::string::npos)
    {
        envMap["SERVER_NAME"] = client.conf["listen"].substr(0, client.conf["listen"].find(":"));
        envMap["SERVER_PORT"] = client.conf["listen"].substr(client.conf["listen"].find(":") + 1);
    }
    else
        envMap["SERVER_PORT"] = client.conf["listen"];
    if (client.req.headers.find("Authorization") != client.req.headers.end())
    {
        pos = client.req.headers["Authorization"].find(" ");
        envMap["AUTH_TYPE"] = client.req.headers["Authorization"].substr(0, pos);
        envMap["REMOTE_USER"] = client.req.headers["Authorization"].substr(pos + 1);
        envMap["REMOTE_IDENT"] = client.req.headers["Authorization"].substr(pos + 1);
    }
    if (client.conf.find("php") != client.conf.end() && client.req.uri.find(".php") != std::string::npos)
        envMap["REDIRECT_STATUS"] = "200";
    std::map<std::string, std::string>::iterator b = client.req.headers.begin();
    while (b != client.req.headers.end())
    {
        envMap["HTTP_" + b->first] = b->second;
        ++b;
    }
    env = (char **)malloc(sizeof(char *) * (envMap.size() + 1));
    std::map<std::string, std::string>::iterator it = envMap.begin();
    int i = 0;
    while (it != envMap.end())
    {
        env[i] = strdup((it->first + "=" + it->second).c_str());
        ++i;
        ++it;
    }
    env[i] = NULL;
    return (env);
}

void		Dispatcher::executeCGI(Client &client)
{
    char			**args = NULL;
    char			**env = NULL;
    std::string		path;
    int				ret;
    int				tubes[2];

    if (client.conf["php"][0] && client.conf["path"].find(".php") != std::string::npos)
        path = client.conf["php"];
    else if (client.conf["exec"][0])
        path = client.conf["exec"];
    else
        path = client.conf["path"];
    close(client.read_fd);
    client.read_fd = -1;
    args = (char **)(malloc(sizeof(char *) * 3));
    args[0] = strdup(path.c_str());
    args[1] = strdup(client.conf["path"].c_str());
    args[2] = NULL;
    env = setCGIEnv(client);
    client.tmp_fd = open(TMP_PATH, O_WRONLY | O_CREAT, 0666);
    pipe(tubes);
    g_logger.log("executing CGI for " + client.ip + ":" + std::to_string(client.port), MED);
    if ((client.cgi_pid = fork()) == 0)
    {
        close(tubes[1]);
        dup2(tubes[0], 0);
        dup2(client.tmp_fd, 1);
        errno = 0;
        ret = execve(path.c_str(), args, env);
        if (ret == -1)
        {
            std::cerr << "Error with CGI: " << strerror(errno) << std::endl;
            exit(1);
        }
    }
    else
    {
        close(tubes[0]);
        client.write_fd = tubes[1];
        client.read_fd = open(TMP_PATH, O_RDONLY);
        client.setFileToWrite(true);
    }
    ft::freeAll(args, env);
}