# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: murachid <murachid@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/17 17:18:51 by murachid          #+#    #+#              #
#    Updated: 2022/03/17 17:18:52 by murachid         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#include "socket.hpp"

NAME	=	webserv
CC		=	c++
CFLAGS	=	-std=c++98
EFLAGS	=	-Wall -Wextra -Werror
DFLAGS	=	-g -fsanitize=address
SRC		=	main.cpp socket/*.cpp

REQSRCS		=	request/request.cpp \
			CGI/cgi.cpp
REQhSRCS	=	request/request.hpp \
			CGI/cgi.hpp

PARSESRCS	=	config/utils.cpp \
				config/src/Config.cpp \
				config/src/ServerConfig.cpp \
				config/src/ServerRoutes.cpp \
				response/response.cpp
PARSEHSRCS	=	config/utils.hpp \
				config/src/Config.hpp \
				config/src/ServerConfig.hpp \
				config/src/ServerRoutes.hpp \
				response/response.hpp

all : $(NAME)

$(NAME)		:	$(SRC) $(PARSESRC) $(PARSEHSRC) $(REQSRCS) $(REQHSRCS)
	@$(CC) $(CFLAGS) $(PARSESRCS) $(REQSRCS) $(SRC) -o $(NAME)
	
clean		:
	@rm -rf $(NAME)

fclean		:	clean
	@rm -rf $(NAME)

re			:	fclean all

test		:	re
	@./$(NAME) config/webserv.conf

.PHONY		:	all clean fclean re