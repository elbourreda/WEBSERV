#ifndef UTILS_H
# define UTILS_H

# include <string>
# include <regex>
# include <fstream>
# include <sstream>
# include <map>
# include <iostream>
// DIR *
# include <dirent.h>
// stat
# include <sys/stat.h>
// setw
# include <iomanip>

# define RESET		"\033[0m"
# define RED		"\033[31m"			/* Red */
# define BOLDRED	"\033[1m\033[31m"	/* Bold Red */

std::string					w_trim(std::string const & s);
std::vector<std::string>	s_split(std::string const & s, char c);
std::string					concat(std::string const & s, int i);
long long int				calculateSize(std::string filename);
void						generate_dirlist(std::string const &, std::string const &, std::string const &);
std::string					searchByExtension(std::string const & extention);

#endif