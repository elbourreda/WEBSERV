#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <vector>
# include <iostream>

# include "ServerRoutes.hpp"
# include "../utils.h"

using namespace std;

class ServerConfig
{
	private:
		int						_body_size_limit;
		vector< string >		_error_pages; // ?
		string					_host;
		string					_name;
		int						_port;
		vector< ServerRoutes >	_routes;

	public:
								ServerConfig( void );
								ServerConfig( ServerConfig const & );
		ServerConfig &			operator=( ServerConfig const & );
								~ServerConfig( void );

		string					getHost( void ) const;
		int						getPort( void ) const;
		int						getBodySizeLimit( void ) const;
		string					getName( void ) const;
		vector< string >		getErrorPages( void ) const;
		ServerRoutes			getRootRoute( void ) const; // throw( string & )
		ServerRoutes			getRoute( int const & ) const; // throw( string & );
		ServerRoutes			getRoute( string ) const; // throw( string & );
		vector< ServerRoutes >	getRoutes( void ) const;
		int						getRouteCount( void ) const;

		void					setHost( string const & );
		void					setPort( int const & );
		void					setBodySizeLimit( int const & );
		void					setName( string const & );
		void					addErrorPage( string const & );
		void					addRoute( ServerRoutes const & );
};

#endif