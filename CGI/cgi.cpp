#include "cgi.hpp" 
#include "../config/src/Config.hpp"

// cgi::cgi(Request &req)
// {
// 	this->reqs = &req;
//     this->outputfile = "/tmp/resp_body";
// 	this->SetCgiEnv();
//     this->SetUpArgs_fds();
// 	this->ExecuteCgi();
// 	std::cout << "\n\n\n-------------end CGI--------\n\n\n";
// }

// void cgi::SetCgiEnv()
// {
// 	setenv("GATEWAY_INTERFACE", "CGI/1.1",1);
// 	setenv("SERVER_PROTOCOL", "HTTP/1.1",1);
// 	setenv("REDIRECT_STATUS", "200",      1);
// 	setenv("SCRIPT_FILENAME", ("/tmp/www" + (*reqs).RequestFile).c_str() , 1);
// 	setenv("REQUEST_METHOD",  (*reqs).method.c_str(), 1); 
// 	setenv("CONTENT_TYPE", (*reqs).Content_type.c_str(), 1);
// 	if ((*reqs).method == "GET")
// 	{
// 		std::cout << "get is in ==> " << ("/tmp/www" + (*reqs).RequestFile) << "\n\n\n";
// 		setenv("QUERY_STRING", (*reqs).QueryString.c_str(), 1);
// 		setenv("CONTENT_LENGTH", "", 1);
// 	}
// 	else
// 	{
// 		this->fd_input = open("body-msg", O_RDONLY); // path of the reques file
// 		setenv("QUERY_STRING", "", 1);
// 		setenv("CONTENT_LENGTH", std::to_string(((*reqs).Content_length)).c_str(), 1);
// 	}
// }

// void cgi::SetUpArgs_fds()
// {
// 	const char*  tmpargs[2] = {
// 		"/Users/rel-bour/goinfre/rel-bour/.brew/bin/php-cgi", // replaced from config file parse  
// 		NULL
//     };
// 	this->args = (char **)tmpargs;
//     // replaced after
//     // this->fd_output = open((this->outputfile).c_str(), O_CREAT | O_WRONLY , 0666);     // path of where you ant to save the body response
//     // this->fd_input = open((*reqs).body_content.c_str(), O_RDONLY);
    
//     // if ((*reqs).method == "POST" && this->fd_input == -1) 
//     //     std::cerr << "ERROR - open() failed" << std::endl;
// }


// void cgi::ExecuteCgi()
// {
// 	// int savein = dup(1);
//     int pid = fork(); 
//     if (pid == 0)
//     {
//         // dup2(this->fd_input, 0); 
//         // dup2(this->fd_output, 1);
//         // execve(this->args[0], this->args, environ);
// 		const char *ss[3] = {"/Users/rel-bour/goinfre/rel-bour/.brew/bin/php-cgi", "/tmp/www/index.php", NULL};

//         execve("/Users/rel-bour/goinfre/rel-bour/.brew/bin/php-cgi", (char **)ss, NULL);

//     }
//     else
//     {
//         waitpid(pid, NULL, 0);
//         // close(this->fd_input);
//         // close(this->fd_output);
// 		// dup2(1, savein);
//     }

// }

// cgi::~cgi()
// {
//     remove((*reqs).body_content.c_str());
// }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// secend

#include "cgi.hpp" 
#define ROOTDIR "/tmp/www" 

cgi::cgi(Request &res, std::string cgi_path)
{
	this->req = &res;
	this->_pathcgi = cgi_path; 
    this->outputfile = "/tmp/resp_body";
	this->SetCgiEnv();
	this->ExecuteCgi();
}

void cgi::SetCgiEnv( void )
{
	setenv("GATEWAY_INTERFACE", "CGI/1.1",1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1",1);
	setenv("REDIRECT_STATUS", "200",      1);
	setenv("SCRIPT_FILENAME", (ROOTDIR+(*this->req).RequestFile).c_str() , 1);
	setenv("REQUEST_METHOD", (*this->req).method.c_str(), 1);

	if ((*this->req).method == "GET")
	{
		setenv("QUERY_STRING", (*this->req).QueryString.c_str(), 1);
		setenv("CONTENT_LENGTH", "", 1);
	}
	else
	{
		setenv("CONTENT_TYPE", ((*this->req).Content_type.erase((*this->req).Content_type.length()-1)).c_str(), 1);
		setenv("QUERY_STRING", "", 1);
		setenv("CONTENT_LENGTH", std::to_string(((*this->req).Content_length)).c_str(), 1);
	}
	this->args[0] = (char *)this->_pathcgi.c_str();
	this->args[1] = NULL;
    this->fd_output = open(this->outputfile.c_str(), O_CREAT | O_WRONLY , 0666);     // path of where you ant to save the body response
    this->fd_input = open((*this->req).body_content.c_str(), O_RDONLY);

}

void cgi::ExecuteCgi( void )
{
    int pid;
	pid = fork();
	if(pid == -1)
		std::cerr << "fial fork\n";
    if (pid == 0)
    {
		dup2(this->fd_input, 0); 
		dup2(this->fd_output, 1);
        execve(this->args[0], this->args, environ);
    }
    else
        waitpid(pid, NULL, 0);
}

cgi::~cgi( void )
{
	close(this->fd_input);
	close(this->fd_output);
    remove((*this->req).body_content.c_str());
}