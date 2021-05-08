#ifndef CLIENT_HPP
# define CLIENT_HPP
# define BUFFER_SIZE 32768
# define TMP_PATH "/tmp/cgi.tmp"
# include "package.hpp"

extern Logger g_logger;

class Client
{
	friend class Server;
	friend class Parser;
    friend class Dispatcher;

	typedef std::map<std::string, std::string> t_conf;
    typedef std::map<std::string, std::string> 	elmt;
    typedef std::map<std::string, elmt>	config;

	struct t_chunk
	{
		unsigned int len;
		bool done;
		bool found;
	};

	public:
		enum status
		{
			BODYPARSING,
			CODE,
			HEADERS,
			CGI,
			BODY,
			RESPONSE,
			STANDBY,
			DONE
		};
		int fd;
		int read_fd;
		int write_fd;
		void readFile();
		void writeFile();

	public:
		Client(int filed, fd_set *r, fd_set *w, struct sockaddr_in info);
		~Client();

		void setReadState(bool state);
		void setWriteState(bool state);
		void setFileToRead(bool state);
		void setFileToWrite(bool state);
		void setToStandBy();
		char *getRBuf(void);

	private:
		int port;
		int status;
		int cgi_pid;
		int tmp_fd;
		char *rBuf;
		fd_set *rSet;
		fd_set *wSet;
		Request req;
		Response res;
		std::string ip;
		std::string last_date;
		std::string response;
		t_conf conf;
		t_chunk chunk;
};

#endif
