#ifndef UTILS_H
# define UTILS_H

#include "package.hpp"
namespace ft
{
	std::string &trim(std::string &str);
	unsigned short ft_htons(unsigned short tmp);
	bool		isspace(int c);
	void		getline(std::string &buffer, std::string &line);
	void		getline(std::string &buffer, std::string &line, char delim);
	int			getpower(int nb, int power);
	std::string	getDate();
	void		freeAll(char **args, char **env);
	int			getMaxFd(std::vector<Server> &servers);
	int			getOpenFd(std::vector<Server> &servers);
	int			FT_FD_ISSET(int _n, fd_set *_p);
	void		FT_FD_ZERO(fd_set *_p);
	void		FT_FD_SET(int _n, fd_set *_p);
	void		FT_FD_CLR(int _n, fd_set *_p);
}

#endif
