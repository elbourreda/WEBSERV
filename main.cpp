#include "socket/socket.hpp"
#include "response/response.hpp"
#include "config/src/Config.hpp"

int		main( int ac, char **av )
{
	string		conf_file;

	if (ac > 2)
		return 0;
	else if (ac == 2)
		conf_file = av[1];
	else
		conf_file = "./config/webserv.conf";
	try
	{
		signal(SIGPIPE, SIG_IGN);
		Config::getInstance().set_config_file(conf_file);
		Config::getInstance().check_config_file();
		Config::getInstance().parse_config();
		_socket socket;
	}
	catch (string & e)
	{
		cout << e;
	}

	return (0);
}