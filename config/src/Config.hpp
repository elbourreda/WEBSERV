#ifndef CONFIG_HPP
# define CONFIG_HPP

// string
# include <string>
// cout
# include <iostream>
// exception
# include <exception>
// ifstream
# include <fstream>

# include "ServerConfig.hpp"
# include "../utils.h"

using namespace std;

class Config
{
	private:
		string					_file;
		vector< ServerConfig >	_servers;
		/**
		 * @brief Construct a new Config object
		 * 
		 * Use Config::getInstance() instead.
		 */
		Config( void );

	public:

		/**
		 * @brief 
		 * 
		 * @param src Config const & - The Config object to copy
		 * @return Config & 
		 */
		Config &				operator=( Config const & ); //

		/**
		 * @brief Destroy the Config object
		 * 
		 */
		~Config( void );

		/**
		 * @brief Get the File object
		 * 
		 * @return string const & 
		 */
		string const &			getFile( void ) const;

		/**
		 * @brief Get the Instance object
		 * 
		 * @return Config & 
		 */
		Config static &			getInstance( void );

		/**
		 * @brief Set the config file object
		 * 
		 * @param file string const & - path to the config file
		 */
		void					set_config_file( string const & file );

		/**
		 * @brief 
		 * 
		 */
		void					check_config_file( void ); // throw( string & );

		/**
		 * @brief Parse the config file object
		 * 
		 */
		void					parse_config( void ); // throw( string & );

		/**
		 * @brief 
		 * 
		 */
		void					validate_config( void ); // throw( string & );

		/**
		 * @brief Get the Servers object
		 * 
		 * @return vector< ServerConfig > const & 
		 */
		vector< ServerConfig > const &	getServers( void ) const;

		/**
		 * @brief Get the Server object
		 * 
		 * @param index int - index of the server
		 * @return ServerConfig const & 
		 */
		ServerConfig const &	getServer( int index ) const; // throw( string & );
		ServerConfig const &	getServer( string const &, int const & ) const;
		ServerConfig const &	getServerName( string const &, int const & ) const;

		int						getServerCount( void ) const;

		/**
		 * @brief Add a server to the config file
		 * 
		 * @param server 
		 */
		void					addServer( ServerConfig const );

		void					line_error(string const &, string const &, int const) const; // throw( string & );

};

#endif