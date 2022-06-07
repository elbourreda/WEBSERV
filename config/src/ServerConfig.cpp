#include "ServerConfig.hpp"

ServerConfig::ServerConfig( void )
{
	this->_host = "";
	this->_port = -1;
	this->_body_size_limit = 1024 * 1024000;
	this->_name = "";
	this->_routes = vector< ServerRoutes >();
	this->_error_204 = "./pages/204.html";
	this->_error_400 = "./pages/400.html";
	this->_error_403 = "./pages/403.html";
	this->_error_404 = "./pages/404.html";
	this->_error_406 = "./pages/406.html";
	this->_error_405 = "./pages/405.html";
	this->_error_413 = "./pages/413.html";
	this->_error_502 = "./pages/502.html";
	this->_error_500 = "./pages/500.html";
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
	this->_routes = src._routes;
	this->_error_204 = src._error_204;
	this->_error_400 = src._error_400;
	this->_error_403 = src._error_403;
	this->_error_404 = src._error_404;
	this->_error_406 = src._error_406;
	this->_error_405 = src._error_405;
	this->_error_413 = src._error_413;
	this->_error_500 = src._error_500;
	this->_error_502 = src._error_502;

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

long long ServerConfig::getBodySizeLimit( void ) const
{
	return this->_body_size_limit;
}

string ServerConfig::getName( void ) const
{
	return this->_name;
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

void ServerConfig::setBodySizeLimit( long long const & b_s_l )
{
	this->_body_size_limit = b_s_l * 1024000;
}

void ServerConfig::setName( string const & name )
{
	this->_name = name;
}

void ServerConfig::addRoute( ServerRoutes const & route )
{
	this->_routes.push_back( route );
}

string ServerConfig::get204Page( void ) const
{
	return this->_error_204;
}

void ServerConfig::set204Page( string const & page )
{
	this->_error_204 = page;
}
string ServerConfig::get400Page( void ) const
{
	return this->_error_400;
}

void ServerConfig::set400Page( string const & page )
{
	this->_error_400 = page;
}

string ServerConfig::get403Page( void ) const
{
	return this->_error_403;
}

void ServerConfig::set403Page( string const & page )
{
	this->_error_403 = page;
}

string ServerConfig::get404Page( void ) const
{
	return this->_error_404;
}

void ServerConfig::set404Page( string const & page )
{
	this->_error_404 = page;
}

string ServerConfig::get405Page( void ) const
{
	return this->_error_405;
}

void ServerConfig::set405Page( string const & page )
{
	this->_error_405 = page;
}

string ServerConfig::get406Page( void ) const
{
	return this->_error_406;
}

void ServerConfig::set406Page( string const & page )
{
	this->_error_406 = page;
}

string ServerConfig::get413Page( void ) const
{
	return this->_error_413;
}

void ServerConfig::set413Page( string const & page )
{
	this->_error_413 = page;
}

string ServerConfig::get500Page( void ) const
{
	return this->_error_500;
}

void ServerConfig::set500Page( string const & page )
{
	this->_error_500 = page;
}

string ServerConfig::get502Page( void ) const
{
	return this->_error_502;
}

void ServerConfig::set502Page( string const & page )
{
	this->_error_502 = page;
}