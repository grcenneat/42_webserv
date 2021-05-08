#include "package.hpp"

namespace ft
{
	std::string &trim(std::string &str)
	{
		if (!str.empty())
		{
			while (isspace(str[0]))
				str.erase(str.begin());
			while (isspace(str[str.size() - 1]))
				str.erase(str.end() - 1);
		}
		return (str);
	}

	unsigned short ft_htons(unsigned short tmp)
	{
		unsigned short inp = (unsigned short)tmp;
		unsigned short a = 0;
		if (BYTE_ORDER == BIG_ENDIAN)
			return (inp);
		for (int i = 0; i < 2; i++)
		{
			a += (inp % 16) << ((i + 2) * 4);
			inp /= 16;
		}
		for (int i = 0; i < 2; i++)
		{
			a += (inp % 16) << (i * 4);
			inp /= 16;
		}
		return (a);
	}


	bool	isspace(int c)
	{
		if (c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f' ||
c == ' ')
			return (true);
		return (false);
	}

	void	getline(std::string &buffer, std::string &line)
	{
		size_t					pos;

		pos = buffer.find("\n");
		if (pos != std::string::npos)
		{
			line = std::string (buffer, 0, pos++);
			buffer = buffer.substr(pos);
		}
		else
		{
			if (buffer[buffer.size() - 1] == '\n')
				buffer = buffer.substr(buffer.size());
			else
			{
				line = buffer;
				buffer = buffer.substr(buffer.size());
			}
		}
	}

	void	getline(std::string &buffer, std::string &line, char delim)
	{
		size_t					pos;

		pos = buffer.find(delim);
		if (pos != std::string::npos)
		{
			line = std::string (buffer, 0, pos++);
			buffer = buffer.substr(pos);
		}
		else
		{
			if (buffer[buffer.size() - 1] == delim)
				buffer = buffer.substr(buffer.size());
			else
			{
				line = buffer;
				buffer = buffer.substr(buffer.size());
			}
		}
	}

	void	freeAll(char **args, char **env)
	{
		free(args[0]);
		free(args[1]);
		free(args);
		int i = 0;
		while (env[i])
		{
			free(env[i]);
			++i;
		}
		free(env);
	}

	std::string		getDate()
	{
		struct timeval	time;
		struct tm		*tm;
		char			buf[BUFFER_SIZE + 1];
		int				ret;

		gettimeofday(&time, NULL);
		tm = localtime(&time.tv_sec);
		ret = strftime(buf, BUFFER_SIZE, "%a, %d %b %Y %T %Z", tm);
		buf[ret] = '\0';
		return (buf);
	}

	int	getMaxFd(std::vector<Server> &servers)
	{
		int		max = 0;
		int		fd;

		for (std::vector<Server>::iterator it(servers.begin()); it != servers.end(); ++it)
		{
			fd = it->getMaxFd();
			if (fd > max)
				max = fd;
		}
		return (max);
	}

	int	getOpenFd(std::vector<Server> &servers)
	{
		int		nb = 0;

		for (std::vector<Server>::iterator it(servers.begin()); it != servers.end(); ++it)
		{
			nb += 1;
			nb += it->getOpenFd();
		}
		return (nb);
	}

	int		getpower(int nb, int power)
	{
		if (power < 0)
			return (0);
		if (power == 0)
			return (1);
		return (nb * getpower(nb, power - 1));
	}

	int FT_FD_ISSET(int _n, fd_set *_p)
	{

	return (_p->fds_bits[(unsigned long)_n/__DARWIN_NFDBITS] & ((__int32_t)(1<<((unsigned long)_n % __DARWIN_NFDBITS))));
	}

	void FT_FD_ZERO(fd_set *_p)
	{
		__builtin_bzero(_p, sizeof(*_p));
	}

	void FT_FD_SET(int _n, fd_set *_p)
	{
		int __fd = _n;
		(_p->fds_bits[(unsigned long)__fd/__DARWIN_NFDBITS] |= ((__int32_t)(1<<(unsigned long)__fd % __DARWIN_NFDBITS)));
	}
	
	void FT_FD_CLR(int _n, fd_set *_p)
	{
		int __fd = _n;
		_p->fds_bits[(unsigned long)__fd/__DARWIN_NFDBITS] &= ~((__int32_t)(1<<(unsigned long)__fd%__DARWIN_NFDBITS));
	}
}

