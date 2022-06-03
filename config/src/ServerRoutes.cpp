#include "ServerRoutes.hpp"

ServerRoutes::ServerRoutes( void )
{
	this->_redirection_code = 301;
	this->_redirection_url = "";
	this->_route = "/";
	this->_root = "/tmp/www";
	this->_dir_list = false;
	this->_phpcgi = "";
	this->_error_403 = "./pages/403.html";
	this->_error_404 = "./pages/404.html";
	this->_error_405 = "./pages/405.html";
}

ServerRoutes::ServerRoutes( ServerRoutes const & src )
{
	*this = src;
}

ServerRoutes & ServerRoutes::operator=( ServerRoutes const & src )
{
	this->_redirection_code = src._redirection_code;
	this->_redirection_url = src._redirection_url;
	this->_route = src._route;
	this->_root = src._root;
	this->_dir_list = src._dir_list;
	this->_indexes = src._indexes;
	this->_methods = src._methods;

	return *this;
}

ServerRoutes::~ServerRoutes( void )
{}

int ServerRoutes::getRedirectionCode( void ) const
{
	return this->_redirection_code;
}

string ServerRoutes::getRedirectionUrl( void ) const
{
	return this->_redirection_url;
}

string ServerRoutes::getRoute( void ) const
{
	return this->_route;
}

string ServerRoutes::getRoot( void ) const
{
	return this->_root;
}

bool ServerRoutes::getDirListing( void ) const
{
	return this->_dir_list;
}

vector<string> ServerRoutes::getIndexes( void ) const
{
	return this->_indexes;
}

string ServerRoutes::getPhpCgi( void ) const
{
	return this->_phpcgi;
}

vector<string> ServerRoutes::getAllMethods( void ) const
{
	return this->_methods;
}

int ServerRoutes::getMethodsCount( void ) const
{
	return this->_methods.size();
}

bool ServerRoutes::getUpload( void ) const
{
	return this->_upload;
}

string ServerRoutes::getUploadDir( void ) const
{
	return this->_upload_dir;
}

bool ServerRoutes::isMethodAllowed( string const & method ) const
{
	return (
		find(this->_methods.begin(), this->_methods.end(), method)
		!=
		this->_methods.end()
	);
}

string ServerRoutes::get403Page( void ) const
{
	return this->_error_404;
}

string ServerRoutes::get404Page( void ) const
{
	return this->_error_404;
}

string ServerRoutes::get405Page( void ) const
{
	return this->_error_404;
}

void ServerRoutes::setRedirectionCode( int const & redirection )
{
	this->_redirection_code = redirection;
}

void ServerRoutes::setRedirectionUrl( string const & redirection )
{
	this->_redirection_url = redirection;
}

void ServerRoutes::setRoute( string const & route )
{
	this->_route = route;
}

void ServerRoutes::setRoot( string const & root )
{
	this->_root = root;
}

void ServerRoutes::setDirListing( bool dir_list )
{
	this->_dir_list = dir_list;
}

void ServerRoutes::addIndex( string const & index )
{
	this->_indexes.push_back(index);
}

void ServerRoutes::setPhpCgi( string const & cgi )
{
	this->_phpcgi = cgi;
}

bool ServerRoutes::addMethod( string const & method )
{
	// is 'method' a valid HTTP method?
	// how to check? (case-insensitive)
	// if (  )
	if ( !this->isMethodAllowed( method ) )
	{
		this->_methods.push_back(method);
		return true;
	}
	return false;
}

void ServerRoutes::setUpload( string const & upload )
{
	if ( upload == "on" )
	{
		this->_upload = true;
	}
	else
	{
		this->_upload = false;
	}
}

void ServerRoutes::setUploadDir( string const & upload )
{
	this->_upload_dir = upload;
}

void ServerRoutes::set403Page( string const & page )
{
	this->_error_404 = page;
}

void ServerRoutes::set404Page( string const & page )
{
	this->_error_404 = page;
}

void ServerRoutes::set405Page( string const & page )
{
	this->_error_404 = page;
}