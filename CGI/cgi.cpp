#include "cgi.hpp" 
#include "../config/src/Config.hpp"

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
	setenv("SCRIPT_FILENAME", ((*this->req).RequestFile).c_str() , 1);
	setenv("REQUEST_METHOD", (*this->req).method.c_str(), 1);

	if ((*this->req).method == "GET")
	{
		setenv("QUERY_STRING", (*this->req).QueryString.c_str(), 1);
		setenv("CONTENT_LENGTH", "", 1);
		setenv("CONTENT_TYPE", "", 1);
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