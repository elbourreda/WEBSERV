#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../request/request.hpp"

extern char **environ;

// class cgi
// {
// 	public:
// 		// Response 
// 		Request *reqs;

// 		// atr
// 		char **envs;
// 		char **args;

// 		int fd_input; 
// 		int fd_output;
// 		std::string outputfile;

// 		// constt
// 		cgi(){};
// 		~cgi();
// 		cgi(Request &reqs);
// 		//methods
// 		void SetCgiEnv(void);
// 		void ExecuteCgi(void);
// 		void SetUpArgs_fds(void);
// };

class cgi
{
	public:
		char	*		args[2];
		char	**		envs;
		int				fd_input;
		int				fd_output;
		std::string		outputfile;
		Request	*		req;
		std::string		_pathcgi;

						cgi ( void ) {};
						cgi ( Request &res, std::string );
						~cgi ( void );
		void			SetCgiEnv( void );
		void			ExecuteCgi( void );
};

extern char **environ;

#endif