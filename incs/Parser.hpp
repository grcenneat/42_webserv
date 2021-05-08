#ifndef PARSER_HPP
# define PARSER_HPP
#include "package.hpp"

typedef std::map<std::string, std::string> 	elmt;
typedef std::map<std::string, elmt>			config;

class Client;

class Parser {
    public:
        Parser();
        ~Parser();
        void		parseRequest(Client &client, std::vector<config> &conf);
        bool		parseHeaders(std::string &buf, Request &req);
        void		parseBody(Client &client);
        void		parseAccept(Client &client, std::multimap<double, std::string> &map, std::string Accept);
        void        parseCGIResult(Client &client);

        bool		checkSyntax(const Request &req);
	    void        getClientConf(Client &client, Request &req, std::vector<config> &conf);

        void		getBody(Client &client);
        void        dechunkBody(Client &client);
        int		    findLen(Client &client);
        void        fillBody(Client &client);
        int			fromHexa(const char *nb);
};

#endif
