#include "package.hpp"

extern	std::vector<Server> g_servers;
extern	bool				g_state;

Config::Config(Dispatcher &dispatcher): _dispatcher(dispatcher)
{

}

Config::~Config()
{

}

void			Config::exit(int sig)
{
	(void)sig;

	std::cout << "\n" << "exiting...\n";
	g_state = false;
}

std::string		Config::readFile(char *file)
{
	int 				fd;
	int					ret;
	char				buf[4096];
	std::string			parsed;

	fd = open(file, O_RDONLY);
	while ((ret = read(fd, buf, 4095)) > 0)
	{
		buf[ret] = '\0';
		parsed += buf;
	}
	close(fd);
	return (parsed);
}

void			Config::parse(char *file, std::vector<Server> &servers)
{
	size_t					d;
	size_t					nb_line;
	std::string				context;
	std::string				buffer;
	std::string				line;
	Server					server(_dispatcher);
	config					tmp;
	bool					http_flag;

	buffer = readFile(file);
	nb_line = 0;
	http_flag = false;
	if (buffer.empty())
		throw(Config::InvalidConfigFileException(nb_line));
	while (!buffer.empty())
	{
		ft::getline(buffer, line);
		nb_line++;
		line = ft::trim(line);
		if (!line.compare(0, 4, "http") && line[line.size() - 1] == '{' && http_flag == false)
		{
			http_flag = true;
			continue;
		}
		if (http_flag == true && line[0] == '}')
			break;

		if (!line.compare(0, 6, "server"))
		{
			while (ft::isspace(line[6]))
				line.erase(6, 1);
			if (line[6] != '{')
				throw(Config::InvalidConfigFileException(nb_line));
			if (!line.compare(0, 7, "server{"))
			{
				d = 7;
				while (ft::isspace(line[d]))
					line.erase(7, 1);
				if (line[d])
					throw(Config::InvalidConfigFileException(nb_line));
				getContent(buffer, context, line, nb_line, tmp);
				std::vector<Server>::iterator it(servers.begin());
				while (it != servers.end())
				{
					if (tmp["server|"]["listen"] == it->_conf.back()["server|"]["listen"])
					{
						std::vector<config>::iterator it2(it->_conf.begin());
						while (it2 != it->_conf.end())
						{
							if (tmp["server|"]["server_name"] == (*it2)["server|"]["server_name"])
								throw(Config::InvalidConfigFileException(nb_line));
							it2++;
						}
						it->_conf.push_back(tmp);
						break ;
					}
					++it;
				}
				if (it == servers.end())
				{
					server._conf.push_back(tmp);
					servers.push_back(server);
				}
				server._conf.clear();
				tmp.clear();
				context.clear();
			}
			else
				throw(Config::InvalidConfigFileException(nb_line));
		}
		else if (line[0])
			throw(Config::InvalidConfigFileException(nb_line));
	}
}

void			Config::getContent(std::string &buffer, std::string &context, std::string prec, size_t &nb_line, config &config)
{
	std::string			line;
	std::string			key;
	std::string			value;
	size_t				pos;
	size_t				tmp;

	prec.pop_back();
	while (prec.back() == ' ' || prec.back() == '\t')
		prec.pop_back();
	context += prec + "|";
	while (ft::isspace(line[0]))
		line.erase(line.begin());
	while (line != "}" && !buffer.empty())
	{
		ft::getline(buffer, line);
		nb_line++;
		line = ft::trim(line);
		if (line[0] != '}')
		{
			pos = 0;
			while (line[pos] && line[pos] != ';' && line[pos] != '{')
			{
				while (line[pos] && !ft::isspace(line[pos]))
					key += line[pos++];
				while (ft::isspace(line[pos]))
					pos++;
				while (line[pos] && line[pos] != ';' && line[pos] != '{')
					value += line[pos++];
			}
			tmp = 0;
			if (line[pos] != ';' && line[pos] != '{')
				throw(Config::InvalidConfigFileException(nb_line));
			else
				tmp++;
			while (ft::isspace(line[pos + tmp]))
				tmp++;
			if (line[pos + tmp])
				throw(Config::InvalidConfigFileException(nb_line));
			else if (line[pos] == '{')
				getContent(buffer, context, line, nb_line, config);
			else
			{
				key = ft::trim(key);
				value = ft::trim(value);
				std::pair<std::string, std::string>	tmp(key, value);
				config[context].insert(tmp);
				key.clear();
				value.clear();
			}

		}
		else if (line[0] == '}' && !buffer.empty())
		{
			pos = 0;
			while (ft::isspace(line[1]))
				line.erase(line.begin() + 1);
			if (line[1])
				throw(Config::InvalidConfigFileException(nb_line));
			context.pop_back();
			context = context.substr(0, context.find_last_of('|') + 1);
		}
	}
	if (line[0] != '}')
		throw(Config::InvalidConfigFileException(nb_line));
}

void			Config::init(fd_set *rSet, fd_set *wSet, fd_set *readSet, fd_set *writeSet, struct timeval *timeout)
{
	signal(SIGINT, exit);
	ft::FT_FD_ZERO(rSet);
	ft::FT_FD_ZERO(wSet);
	ft::FT_FD_ZERO(readSet);
	ft::FT_FD_ZERO(writeSet);
	timeout->tv_sec = 1;
	timeout->tv_usec = 0;

	for (std::vector<Server>::iterator it(g_servers.begin()); it != g_servers.end(); ++it)
		it->init(rSet, wSet);
}

Config::InvalidConfigFileException::InvalidConfigFileException(void) {this->line = 0;}

Config::InvalidConfigFileException::InvalidConfigFileException(size_t d) {
	this->line = d;
	this->error = "line " + std::to_string(this->line) + ": Invalid Config File";
}

Config::InvalidConfigFileException::~InvalidConfigFileException(void) throw() {}

const char					*Config::InvalidConfigFileException::what(void) const throw()
{
	if (this->line)
		return (error.c_str());
	return ("Invalid Config File");
}
