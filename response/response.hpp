#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// # include <iostream>
# include <sys/types.h>
# include <sys/stat.h>
// # include <fcntl.h>
// # include <unistd.h>
// # include <stdlib.h>
// # include <string> 
// # include <fstream>
// # include <sstream>
# include <cstring>
# include <libgen.h>
# include "../config/src/Config.hpp"
# include "../config/utils.h"
# include "../request/request.hpp"
# include "../CGI/cgi.hpp"

class Request;

// # Generate Response
// 	- prepare the head response 
// 		- first line need:
// 			* status code
// 		- Content-Type
// 		- Content-Length
// 	- need the path of the requested file
// 	- if the file with .php extention will call (CGI)
// 	- if the file its not php file try to read it 
// 	- get the Length and Type for the head response
// 	- write the file into the fd of the socket accepted

class Response
{
	private:
		Request			_req;
		int				_fd_out;
		long long		timestamp;
						Response ( void );
		void			start ( void );
		void			send_file ( void );
		void			output_file ( ServerRoutes const & );

	public:
		int				statusCode;
		int				isAutoindex;
		std::string		responseFile;

						Response ( int &, Request & );
		ServerRoutes	which_route ( ServerConfig, string const & );
		void			which_file( ServerConfig const &, ServerRoutes const &, string const & );
		std::string		getContentType ( std::string & );
		int				deleteMethod( std::string const & path, std::string const & rootdir );
		int				getLengthFileCgi(std::string outputfile);
};

#endif // !REQUEST_HPP