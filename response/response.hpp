#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// # include <iostream>
# include <sys/types.h>
# include <sys/stat.h>
// # include <fcntl.h>
// # include <unistd.h>
// # include <stdlib.h>
// # include <vector>
// # include <string> 
// # include <fstream>
// # include <sstream>
# include <cstring>
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
						Response ( void );
		void			start ( void );
		long long		timestamp;
		void			output_file ( ServerRoutes const & );

	public:
		int				statusCode;
		int				isAutoindex;
		std::string		responseFile;

						Response ( int &, Request & );
		void			send_file ( void );
		// void			GetStatusCode();
		// int				AutondexOn();
		// void			SuccessResponse();
		// std::string		getRepnsFirstline(int statuscode);
		std::string		getContentType ( std::string & );
		// std::string		getRepnsContLength(int Length);
};

#endif // !REQUEST_HPP