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
		route = server.getRoute(path);
	// path contains a filename
	else
	{
		// in case the path was /file.html
		if ( paths_splitted.size() == 1 )
			route = server.getRoute("/");
		// multiple subdirectories
		else
			// remove last / that points to file
			route = server.getRoute(path.substr(0, path.rfind('/')));
	}
	/*	WHICH ROUTE	*/


	/*	REQUESTED FILE AND AUTOINDEX	*/
	// CONTAINS DOT NOT
	struct stat info;
	if ( paths_splitted.back().find('.') == string::npos )
	{
		string requested_dir = route.getRoot() + path;
		if ( stat(requested_dir.c_str(), &info) != 0 )
		{
			this->statusCode = 404;
			this->responseFile = server.get404Page();
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

					if ( this->responseFile.size() > 4 &&
						 this->responseFile.substr(this->responseFile.size() - 4) == ".php" )
						this->_req.IsPhpFile = true;
				}
				else
				{
					this->statusCode = 403;
					this->responseFile = server.get403Page();
				}
			}
		}
		else
		{
			this->statusCode = 404;
			this->responseFile = server.get404Page();
		}
	}

	// CONTAINS DOT
	else
	{
		string requested_file = route.getRoot() + path;

		if ( stat(requested_file.c_str(), &info) != 0)
		{
			this->statusCode = 404;
			this->responseFile = server.get404Page();
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

						if ( this->responseFile.size() > 4 &&
							 this->responseFile.substr(this->responseFile.size() - 4) == ".php" )
							this->_req.IsPhpFile = true;
					}
					else
					{
						this->statusCode = 403;
						this->responseFile = server.get403Page();
					}
				}
			}
		}
	}
	/*	REQUESTED FILE AND AUTOINDEX	*/

	/*	CGI AVAILABILITY	*/
	if ( this->_req.IsPhpFile && route.getPhpCgi() != "" )
	{
		// check if php-cgi is a file
		struct stat info;
		if ( !(stat(route.getPhpCgi().c_str(), &info) != 0
			&& info.st_mode & S_IFREG) )
		{
			this->statusCode = 502;
			this->responseFile = server.get502Page();
		}
	}
	/*	CGI AVAILABILITY	*/

	/*		*/
	if ( this->_req.Content_length > server.getBodySizeLimit() )
	{
		this->statusCode = 413;
		this->responseFile = server.get413Page();
	}
	/*		*/

	/*	REDIRECTION	*/
	if ( (route.getRedirectionCode() == 301 || route.getRedirectionCode() == 302)
		&& route.getRedirectionUrl() != "" )
	{
		string header;
		header = "HTTP/1.1 " + getStatusByCode(route.getRedirectionCode()) + "\r\n";
		write(this->_fd_out, header.c_str(), header.size());
		header = "Location: " + route.getRedirectionUrl() + "\r\n";
		write(this->_fd_out, header.c_str(), header.size());
	}
	/*	REDIRECTION	*/

	/*	METHOD CHECK	*/
	if (route.isMethodAllowed(this->_req.method) == false)
	{
		this->statusCode = 405;
		cout << server.get405Page() << endl;
		this->responseFile = server.get405Page();
	}
	/*	METHOD CHECK	*/

	/*	400	*/
	/* loop through method check if char is lowercase */
	for ( int i = 0; i < this->_req.method.size(); i++ )
	{
		if ( islower(this->_req.method[i]) )
		{
			this->statusCode = 400;
			this->responseFile = server.get400Page();
			break;
		}
	}
	/*	400	*/

	this->output_file( route );

	this->send_file();

	// cout << "Removing " << concat("workdir/response_", this->timestamp) << endl;
	remove(concat("workdir/response_", this->timestamp).c_str());
	// cout << "Removing " << concat("workdir/dirlist_", this->timestamp) << endl;
	remove(concat("workdir/dirlist_", this->timestamp).c_str());
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
		if (this->statusCode == 200)
		{
			if ( this->_req.IsPhpFile && route.getPhpCgi() != "" )
			{
				// PHP CGI
				end_file << "HTTP/1.1 " << "200 OK" << "\r\n";
				end_file << "Content-Length: " << calculateSize(this->responseFile) << "\r\n";

				cgi _cgi( this->_req, route.getPhpCgi() );
				this->responseFile = _cgi.outputfile;

				ifstream i_file(this->responseFile, ios::binary);
				if (i_file)
				{
					end_file << i_file.rdbuf();
				}
				i_file.close();

				remove(this->responseFile.c_str());
			}

			else
			{
				// HTML
				end_file << "HTTP/1.1 " << "200 OK" << "\r\n";
				end_file << "Content-Length: " << calculateSize(this->responseFile) << "\r\n";
				end_file << "Content-Type: " << getContentType(this->responseFile) << "\r\n";
				end_file << "\r\n";

				ifstream i_file(this->responseFile, ios::binary);
				if (i_file)
				{
					end_file << i_file.rdbuf();
				}
				i_file.close();
			}
		}

		else if (this->statusCode != 301 || this->statusCode != 302)
		{
			// HTML
			end_file << "HTTP/1.1 " << getStatusByCode(this->statusCode) << "\r\n";
			end_file << "Content-Length: " << calculateSize(this->responseFile) << "\r\n";
			end_file << "Content-Type: " << getContentType(this->responseFile) << "\r\n";
			end_file << "\r\n";

			ifstream i_file(this->responseFile, ios::binary);
			if (i_file)
			{
				end_file << i_file.rdbuf();
			}
			i_file.close();
		}

		end_file.close();
	}
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