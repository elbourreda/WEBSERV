#include "response.hpp"

std::string		path_it(std::string const & filename, std::string const & routePath)
{
	size_t			pos = filename.find(routePath);
	std::string		path = routePath;
	if (path[path.size() - 1] != '/')
		path += "/";

	if ( pos != std::string::npos )
	{
		std::string newFilename = filename.substr(0, pos);
		newFilename += filename.substr(pos + routePath.length());
		if ( newFilename.length() > 0 )
			return newFilename;
		else
			return "/";
	}
	else
		return filename;
}

Response::Response( void ) {}

Response::Response( int & fd, Request & request )
{
	this->_fd_out = fd;
	this->_req = request;
	this->responseFile = "";
	this->statusCode = 200;
	this->timestamp = time(NULL);
	this->start();
}

ServerRoutes	Response::which_route( ServerConfig server, string const & path )
{
	ServerRoutes	route;
	vector<string>	paths_splitted;

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
			route = server.getRoute("/");
		// multiple subdirectories
		else
			// remove last / that points to file
			route = server.getRoute(path.substr(0, path.rfind('/')));
	}
	return ( route );
}

void	Response::which_file( ServerConfig const & server, ServerRoutes const & route, string const & path, string const & o_path )
{
	// CONTAINS DOT NOT
	string	myRoot = ( route.getUploadDir() == "" ? route.getRoot() : route.getUploadDir() );

	string	requested_file = myRoot;
	if (requested_file[requested_file.length() - 1] != '/')
		requested_file += "/";
	requested_file += path;

	if ( !file_exists(requested_file) && !directory_exists(requested_file) )
	{
		this->statusCode = 404;
		this->responseFile = server.get404Page();
	}
	else if ( hasAccess(requested_file) == -1 )
	{
		this->statusCode = 403;
		this->responseFile = server.get403Page();
	}
	else
	{
		// FILE
		if ( file_exists(requested_file) )
		{
			this->statusCode = 200;
			this->responseFile = requested_file;
		}
		// DIRECTORY
		else
		{
			// AUTOINDEXING
			for ( int i = 0; i < route.getIndexes().size(); i++ )
			{
				string index_file = myRoot + path + "/" + route.getIndexes()[i];
				if ( file_exists( index_file ) )
				{
					this->statusCode = 200;
					this->responseFile = index_file;
					this->_req.RequestFile = index_file;

					if ( this->responseFile.size() > 4 &&
						this->responseFile.substr(this->responseFile.size() - 4) == ".php" )
						this->_req.IsPhpFile = true;

					break;
				}
			}
			// IF NO INDEX FOUND AND DIRLIST, THEN LIST DIRECTORIES
			if ( this->responseFile == "" )
			{
				if ( route.getDirListing() )
				{
					this->statusCode = 200;
					string output_file_name = concat("/tmp/.dirlist_", this->timestamp);
					output_file_name += ".html";
					generate_dirlist(output_file_name, myRoot + path, o_path);
					this->responseFile = output_file_name;
				}
			}
			// IF NOT 403
			if ( this->responseFile == "" && route.getDirListing() == false )
			{
				this->statusCode = 403;
				this->responseFile = server.get403Page();
			}
		}
	}
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
	string			original_path = this->_req.RequestFile, path = this->_req.RequestFile;
	int				port = atoi(this->_req.port.c_str());
	ServerConfig	server;
	ServerRoutes	route;

	server = Config::getInstance().getServer(host, port);

	/*	WHICH ROUTE	*/
	route = this->which_route( server, path );
	/*	WHICH ROUTE	*/

	if ( route.getUploadDir() == "" )
	{
		path = this->_req.RequestFile = path_it(this->_req.RequestFile, route.getRoute());
	}
	else
	{
		path = this->_req.RequestFile = path_it(this->_req.RequestFile, route.getUploadDir());
	}


	/*	REQUESTED FILE AND AUTOINDEX	*/
	which_file( server, route, path, original_path );
	/*	REQUESTED FILE AND AUTOINDEX	*/

	/*	502 CGI AVAILABILITY	*/
	if ( this->_req.IsPhpFile && route.getPhpCgi() != "" ) 
	{
		// check if php-cgi is a file
		if ( !file_exists(route.getPhpCgi()) )
		{
			this->statusCode = 502;
			this->responseFile = server.get502Page();
		}
	}
	/*	502 CGI AVAILABILITY	*/

	/*	413 BODY LIMIT	*/
	if ( this->_req.Content_length > server.getBodySizeLimit() )
	{
		this->statusCode = 413;
		this->responseFile = server.get413Page();
	}
	/*	413 BODY LIMIT	*/

	/*	405 METHOD CHECK	*/
	if (route.isMethodAllowed(this->_req.method) == false)
	{
		this->statusCode = 405;
		this->responseFile = server.get405Page();
	}
	/*	405 METHOD CHECK	*/

	/*	201	*/
	if ( route.getUploadDir() != "" && this->_req.method == "POST" )
	{
		if ( this->statusCode == 404 )
		{
			// rename uploaded file to requested file
			string up_path = dirname((char *)(route.getUploadDir() + "/" + this->_req.RequestFile).c_str());

			if ( directory_exists(up_path) )
			{
				ifstream	i_file(this->_req.body_content, ios::binary);
				ofstream	end_file(route.getUploadDir() + "/" + this->_req.RequestFile, ios::binary);
				if (end_file && i_file)
				{
					end_file << i_file.rdbuf();
				}
				i_file.close();
				end_file.close();

				this->statusCode = 201;
				this->responseFile = "";
			}

			else
			{
				this->statusCode = 404;
				this->responseFile = server.get404Page();
			}
		}
		else
		{
			if ( !this->_req.IsPhpFile )
			{
				this->statusCode = 406;
				this->responseFile = server.get406Page();
			}
		}
		remove(this->_req.body_content.c_str());
	}
	/*	201	*/

	/*	30X REDIRECTION	*/
	if ( (route.getRedirectionCode() == 301 || route.getRedirectionCode() == 302)
		&& route.getRedirectionUrl() != ""
		&& this->statusCode == 200 )
	{
		string header;
		header = "HTTP/1.1 " + getStatusByCode(route.getRedirectionCode()) + "\r\n";
		write(this->_fd_out, header.c_str(), header.size());
		header = "Location: " + route.getRedirectionUrl() + "\r\n";
		write(this->_fd_out, header.c_str(), header.size());
	}
	/*	30X REDIRECTION	*/

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

	/*	DELETE	*/
	if ( this->_req.method == "DELETE" && this->statusCode != 405 )
	{
		this->statusCode = deleteMethod(this->_req.RequestFile, route.getRoot());
		if ( this->statusCode == 204 )
			this->responseFile = "";
		else if ( this->statusCode == 403 )
			this->responseFile = server.get403Page();
		else if ( this->statusCode == 404 )
			this->responseFile = server.get404Page();
		else if ( this->statusCode == 500 )
			this->responseFile = server.get500Page();
	}
	/*	DELETE	*/

	this->output_file( route );

	this->send_file();

	remove(concat("/tmp/.response_", this->timestamp).c_str());
	remove((concat("/tmp/.dirlist_", this->timestamp) + ".html").c_str());
}

void	Response::output_file( ServerRoutes const & route )
{
    ofstream   end_file;
	string     file_name;

	file_name = concat("/tmp/.response_", this->timestamp);
	end_file.open(file_name.c_str());

	if ( end_file.is_open() )
	{
		if (this->statusCode >= 200 && this->statusCode <= 204)
		{
			if ( this->_req.IsPhpFile && route.getPhpCgi() != "" && this->statusCode != 201 && this->statusCode != 204 )
			{
				// PHP CGI
				this->_req.RequestFile = this->responseFile;

				end_file << "HTTP/1.1 " << getStatusByCode(200) << "\r\n";

				cgi _cgi( this->_req, route.getPhpCgi() );
				this->responseFile = _cgi.outputfile;

				end_file << "Content-Length: " << getLengthFileCgi(this->responseFile) << "\r\n";
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
		}

		else if (this->statusCode != 301 && this->statusCode != 302)
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

	file_name = concat("/tmp/.response_", this->timestamp);

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

bool	detectIndexRoute(string const & name)
{
	for (int i = 0; i < name.size(); i++)
		if (name[i] != '/')
			return (false);
	return (true);
}

// 204, 403, 404, 500
int		Response::deleteMethod(string const & path, string const & rootdir)
{
	std::string fullpath = rootdir;
	if (fullpath[fullpath.size() - 1] != '/')
		fullpath += "/";
	fullpath += path;

	if (fullpath[fullpath.size() - 1] == '/' && !(detectIndexRoute(path)))
	{
		while (fullpath[fullpath.size() - 1] == '/')
			fullpath.pop_back();
	}

	if (detectIndexRoute(path))
	{
		// status code here allways 403 forbiden *
		return 403;
	}

	else
	{
		// not found
		if (!directory_exists(fullpath) && !file_exists(fullpath))
			return 404;

		// permission denied
		else if (access(fullpath.c_str(), W_OK) == -1)
			return 403;

		// else
		else
		{
			// directory
			if (directory_exists(fullpath.c_str()))
			{
				return 403;
			}

			// file
			else
			{
				if (remove(fullpath.c_str()) == 0)
					return 204;
				else
					return 500;
			}
		}
	}
}

int		Response::getLengthFileCgi(std::string outputfile)
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