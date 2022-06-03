#include "ServerConfig.hpp"

ServerConfig::ServerConfig( void )
{
	this->_host = "";
	this->_port = -1;
	this->_body_size_limit = 1024;
	this->_name = "";
	this->_error_pages = vector< string >();
	this->_routes = vector< ServerRoutes >();
}

ServerConfig::ServerConfig( ServerConfig const & src )
{
	*this = src;
}

ServerConfig & ServerConfig::operator=( ServerConfig const & src )
{
	this->_host = src._host;
	this->_port = src._port;
	this->_body_size_limit = src._body_size_limit;
	this->_name = src._name;
	this->_error_pages = src._error_pages;
	this->_routes = src._routes;

	return *this;
}

ServerConfig::~ServerConfig( void )
{}

string ServerConfig::getHost( void ) const
{
	return this->_host;
}

int ServerConfig::getPort( void ) const
{
	return this->_port;
}

string ServerConfig::getName( void ) const
{
	return this->_name;
}

vector< string > ServerConfig::getErrorPages( void ) const
{
	return this->_error_pages;
}

ServerRoutes		ServerConfig::getRootRoute( void ) const // throw( string & )
{
	for (int i = 0; i < this->getRouteCount(); i++)
	{
		if ( this->_routes[i].getRoute() == "/" )
		{
			return (this->_routes[i]);
		}
	}
	throw string(RED + string("") + "Error: No Route with root directory '/' exists.\n" + string("") + RESET);
}

ServerRoutes		ServerConfig::getRoute( int const & index ) const // throw( string & )
{
	if ( index >= 0 && index < this->getRouteCount() )
	{
		return (this->_routes[index]);
	}
	throw string(RED + string("") + "Error: Server index out of range.\n" + string("") + RESET);
}

ServerRoutes		ServerConfig::getRoute( string path ) const // throw( string & )
{
	while ( count(path.begin(), path.end(), '/') > 0 )
	{
		for (int i = 0; i < this->getRouteCount(); i++)
		{
			if ( this->_routes[i].getRoute() == path )
			{
				return (this->_routes[i]);
			}
		}
		path = path.substr(0, path.rfind('/'));
	}
	return this->getRootRoute();
}

vector< ServerRoutes > ServerConfig::getRoutes( void ) const
{
	return this->_routes;
}

int ServerConfig::getRouteCount( void ) const
{
	return this->_routes.size();
}

void ServerConfig::setHost( string const & host )
{
	this->_host = host;
}

void ServerConfig::setPort(int const & port )
{
	this->_port = port;
}

void ServerConfig::setBodySizeLimit( int const & b_s_l )
{
	this->_body_size_limit = b_s_l;
}

void ServerConfig::setName( string const & name )
{
	this->_name = name;
}

void ServerConfig::addErrorPage( string const & error_page )
{
	this->_error_pages.push_back( error_page );
}

void ServerConfig::addRoute( ServerRoutes const & route )
{
	this->_routes.push_back( route );
}