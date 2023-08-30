# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tbouzalm <tbouzalm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/17 06:08:14 by tbouzalm          #+#    #+#              #
#    Updated: 2023/08/29 14:01:06 by tbouzalm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

CPP = c++ 

FLAGS = -Wall -Wextra -Werror -std=c++98

OBJ =  $(SRC:.cpp=.o)

SRC = main2.cpp server.cpp request.cpp 
# SRC1 = test_conf.cpp

all : $(NAME)

%.o: %.cpp server.hpp
	$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CPP) $(FLAGS) $(OBJ) -o $(NAME)

clean : 
		rm -f $(OBJ)

fclean : clean
		rm -rf $(NAME)

re : fclean all

.PHONY : clean fclean