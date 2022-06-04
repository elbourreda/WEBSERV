#ifndef SERVERROUTES_HPP
# define SERVERROUTES_HPP

# include <string>
# include <vector>

using namespace std;

class ServerRoutes
{
	private:
		string			_phpcgi;
		bool			_dir_list;
		vector<string>	_indexes;
		vector<string>	_methods;
		int				_redirection_code;
		string			_redirection_url;
		string			_root;
		string			_route;
		bool			_upload;
		string			_upload_dir;

	public:
						ServerRoutes( void );
						ServerRoutes( ServerRoutes const & );
		ServerRoutes &	operator=( ServerRoutes const & );
						~ServerRoutes( void );

		int				getRedirectionCode( void ) const;
		string			getRedirectionUrl( void ) const;
		string			getRoute( void ) const;
		string			getRoot( void ) const;
		bool			getDirListing( void ) const;
		vector<string>	getIndexes( void ) const;
		string			getPhpCgi( void ) const;
		vector<string>	getAllMethods( void ) const;
		int				getMethodsCount( void ) const;
		bool			isMethodAllowed( string const & ) const;
		bool			getUpload( void ) const;
		string			getUploadDir( void ) const;

		void			setRedirectionCode( int const & );
		void			setRedirectionUrl( string const & );
		void			setRoute( string const & );
		void			setRoot( string const & );
		void			setDirListing( bool );
		void			addIndex( string const & );
		void			setPhpCgi( string const & );
		bool			addMethod( string const & ); //
		void			setUpload( string const & );
		void			setUploadDir( string const & );
};

#endif