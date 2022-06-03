#include "response.hpp"

Response::Response( void )
{}

Response::Response( int & fd, Request & request )
{
	this->_fd_out = fd;
	this->_req = request;
	this->responseFile = "";
	this->statusCode = 200;
	this->timestamp = time(NULL);
	this->start();
}

/*
	400 Bad Request
	401 Unauthorized						U
	402 Payment Required					U
	403 Forbidden
	404 Not Found							X
	405 Method Not Allowed					X
	413 Request Entity Too Large
	415 Unsupported Media Type

	500 Internal Server Error
	501 Not Implemented
	505 HTTP Version Not Supported
	503 Service Unavailable

	200 OK
	201 Created (upload was successful)
	204 No Content (used for DELETE requests)

	301
	302
*/
void		Response::start( void )
{
	/*
		127.0.0.1	/
		127.0.0.1	/	81

		127.0.0.2	/

		127.0.0.3	/path
		127.0.0.3	/path/file.html
	*/

// 1 location
// 2 path

	string			host = this->_req.host;
	string			path = this->_req.RequestFile;
	int				port = atoi(this->_req.port.c_str());
	ServerConfig	server;
	ServerRoutes	route;
	vector<string>	paths_splitted;

	server = Config::getInstance().getServer(host, port);

	/*	WHICH ROUTE	*/
	// path does not contain a filename
	paths_splitted = s_split(path, '/');
	
	if ( paths_splitted.back().find('.') == string::npos )
	{
		route = server.getRoute(path);
	}
	// path contains a filename
	else
	{
		// in case the path was /file.html
		if ( paths_splitted.size() == 1 )
		{
			route = server.getRoute("/");
		}
		// multiple subdirectories
		else
		{
			// remove last / that points to file
			route = server.getRoute(path.substr(0, path.rfind('/')));
		}
	}
	/*	WHICH ROUTE	*/


	/*	REQUESTED FILE AND AUTOINDEX	*/
	// CONTAINS DOT NOT
	struct stat info;
	if ( paths_splitted.back().find('.') == string::npos )
	{
		string requested_dir = route.getRoot() + path;
		if ( stat(requested_dir.c_str(), &info) != 0)
		{
			this->statusCode = 404;
			this->responseFile = route.get404Page();
		}
		else if( info.st_mode & S_IFDIR )
		{
			for ( int i = 0; i < route.getIndexes().size(); i++ )
			{
				string index_file = route.getRoot() + path + "/" + route.getIndexes()[i];
				if ( stat(index_file.c_str(), &info) == 0 )
				{
					this->responseFile = index_file;
					break;
				}
			}
			if ( this->responseFile == "" )
			{
				if ( route.getDirListing() )
				{
					this->statusCode = 200;
					this->isAutoindex = true;
					string output_file_name = concat("workdir/dirlist_", this->timestamp);
					generate_dirlist(output_file_name, route.getRoot() + path, this->_req.RequestFile);
					this->responseFile = output_file_name;
				}
				else
				{
					this->statusCode = 403;
					this->responseFile = route.get403Page();
				}
			}
		}
		else
		{
			this->statusCode = 404;
			this->responseFile = route.get404Page();
		}
	}

	// CONTAINS DOT
	else
	{
		string requested_file = route.getRoot() + path;

		if ( stat(requested_file.c_str(), &info) != 0)
		{
			this->statusCode = 404;
			this->responseFile = route.get404Page();
		}
		else
		{
			// FILE
			if ( info.st_mode & S_IFREG )
			{
				this->responseFile = requested_file;
			}
			// FOLDER
			else if( info.st_mode & S_IFDIR )
			{
				for ( int i = 0; i < route.getIndexes().size(); i++ )
				{
					string index_file = route.getRoot() + path + "/" + route.getIndexes()[i];
					if ( stat(index_file.c_str(), &info) == 0 )
					{
						this->responseFile = index_file;
						break;
					}
				}
				if ( this->responseFile == "" )
				{
					// no index file found
					if ( route.getDirListing() )
					{
						this->statusCode = 200;
						this->isAutoindex = true;
						string output_file_name = concat("workdir/dirlist_", this->timestamp);
						generate_dirlist(output_file_name, route.getRoot() + path, this->_req.RequestFile);
						this->responseFile = output_file_name;
					}
					else
					{
						this->statusCode = 403;
						this->responseFile = route.get403Page();
					}
				}
			}
		}
	}
	/*	REQUESTED FILE AND AUTOINDEX	*/


	/*	METHOD CHECK	*/
	if (route.isMethodAllowed(this->_req.method) == false)
	{
		this->statusCode = 405;
		this->responseFile = route.get405Page();
	}
	/*	METHOD CHECK	*/

	this->output_file( route );

	this->send_file();
}

/*
	Check content length with body limit
*/

int GetLengthFileCgi(std::string outputfile)
{
	std::string line;
	std::ifstream content(outputfile);
	int len;

	len = 0;
	while (getline (content, line) && line != "\r")
		;
	while (getline (content, line))
		len += line.size() + 1;
	return (len);
}

void	Response::output_file( ServerRoutes const & route )
{
    ofstream   end_file;
	string     file_name;

	file_name = concat("workdir/response_", this->timestamp);
	end_file.open(file_name.c_str());

	if ( end_file.is_open() )
	{
		end_file << "HTTP/1.1 " << getStatusByCode(this->statusCode) << "\r\n";

		// if cgi
		if ( this->_req.IsPhpFile /*&& route.getPhpCgi() != ""*/ )
		{
			// cout << "CGI" << endl;
			// execute cgi
			cgi _cgi( this->_req, route.getPhpCgi() );
			// calculate and cgi outputfile size without header
			/*	REDA	*/
			end_file << "Content-Length: " << (GetLengthFileCgi(_cgi.outputfile)) << "\r\n";
			// std::cout << "len of cgi body ==> " << GetLengthFileCgi(_cgi.outputfile) << "\n";
		}

		// not cgi
		else
		{
			// cout << "NOT CGI" << endl;
			// calculate and output header length
			end_file << "Content-Length: " << calculateSize(this->responseFile) << "\r\n";
			end_file << "Content-Type: " << getContentType(this->responseFile) << "\r\n";
			end_file << "\r\n";
		}


		{
			// copy response file to end_file
			/* reda */
			std::string FinalResponseFile;
			if (!this->_req.IsPhpFile)
				FinalResponseFile = this->responseFile.c_str();
			else
				FinalResponseFile = "/tmp/resp_body";

			ifstream i_file(FinalResponseFile, ios::binary);				
			// ifstream i_file(this->responseFile.c_str(), ios::binary);				
			if (i_file)
			{
				end_file << i_file.rdbuf();
			}
			i_file.close();
			if (this->_req.IsPhpFile)
				remove("/tmp/resp_body");
			
		}
	}

	end_file.close();
}

// write response file to client
void	Response::send_file( void )
{
	string file_name;

	file_name = concat("workdir/response_", this->timestamp);

	ifstream myfile(file_name.c_str());
	if (myfile.is_open())
	{
		string line;
		while ( getline(myfile, line) )
		{
			write(this->_fd_out, line.c_str(), line.size());
			write(this->_fd_out, "\n", 1);
		}
		myfile.close();
	}
}

std::string Response::getContentType(std::string & filename)
{
    std::size_t n = filename.rfind('.');
    if (n != std::string::npos)
    {
        std::string ext = filename.substr(n+1);
        return (searchByExtension(ext));
    }
    return ("text/html");
}