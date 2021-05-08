#include "package.hpp"

std::vector<Server>		g_servers;
bool					g_state = true;
Logger					g_logger(1, "console", LOW);

int		ret_error(std::string error)
{
	std::cerr << error << std::endl;
	return (1);
}

int 	main(int ac, char **av)
{
	Config					config(*Dispatcher::GetInstance());
	fd_set					readSet;
	fd_set					writeSet;
	fd_set					rSet;
	fd_set					wSet;
	struct timeval			timeout;
	Client					*client;

	if (ac != 2)
		return (ret_error("Usage: ./webserv config-file"));
	try
	{
		config.parse(av[1], g_servers);
		config.init(&rSet, &wSet, &readSet, &writeSet, &timeout);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	while (g_state)
	{
		readSet = rSet;
		writeSet = wSet;

		select(ft::getMaxFd(g_servers) + 1, &readSet, &writeSet, NULL, &timeout);

		for (std::vector<Server>::iterator s(g_servers.begin()); s != g_servers.end(); ++s)
		{
			if (ft::FT_FD_ISSET(s->getFd(), &readSet))
			{
				try
				{
					if (!g_state)
						break ;
					if (ft::getOpenFd(g_servers) > MAX_FD)
						s->refuseConnection();
					else
						s->acceptConnection();
				}
				catch (std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
				}
			}
			if (!s->_503_clients.empty())
			{

				if (ft::FT_FD_ISSET(s->_503_clients.front(), &writeSet))
					s->send503(s->_503_clients.front());
			}

			for (std::vector<Client*>::iterator c(s->_clients.begin()); c != s->_clients.end(); ++c)
			{
				client = *c;
				if (ft::FT_FD_ISSET(client->fd, &readSet))
					if (!s->readRequest(c))
						break ;
				if (ft::FT_FD_ISSET(client->fd, &writeSet))
					if (!s->writeResponse(c))
						break ;
				if (client->write_fd != -1)
					if (ft::FT_FD_ISSET(client->write_fd, &writeSet))
						client->writeFile();
				if (client->read_fd != -1)
					if (ft::FT_FD_ISSET(client->read_fd, &readSet))
						client->readFile();
			}
		}
	}
	g_servers.clear();
	return(0);
}
