# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dcarvalh <dcarvalh@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/01 00:09:19 by psoares-          #+#    #+#              #
#    Updated: 2024/04/23 18:16:24 by dcarvalh         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



NAME = ircserv
CXX = c++
CXXFLAGS =  -g -std=c++98 -Wall -Wextra -Werror

SRCS =	Source/Server.cpp Source/PollConnection.cpp Source/Main.cpp Source/Client.cpp Source/Channel.cpp Source/IrcMessage.cpp Source/Modes.cpp Source/Bot.cpp

Include = -IInclude

OBJS = $(SRCS:.cpp=.o)

C_RED = \033[0;31m
C_GREEN = \033[1;92m
C_RESET = \033[0m
C_PURPLE = \033[0;35m
C_RED = \033[0;31m
BG_YELLOW = \x1b[43m
echo = /bin/echo -e



$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(Include) $(OBJS) -o $(NAME)
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(Include) -c $< -o $@

all : $(NAME)
	
fclean:
	@rm -f $(NAME) $(OBJS)
	@$(echo) "$(C_RED)\tRemoved $(NAME)$(C_RESET)"
	
re: fclean all

gadd : $(SRCS)
	@git add $(SRCS) Include/*.hpp Makefile
	@$(echo) "$(C_GREEN) SRCS added to git $(C_RESET)"

git : gadd
	@git commit -am "make commit"
	@$(echo) "$(C_PURPLE) SRCS committed $(C_RESET)"
	@git push origin master
	@$(echo) "$(C_GREEN) PUSHED $(C_RESET)" $(NAME)


run: re
	./$(NAME) 8080 a