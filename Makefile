NAME = webserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -fsanitize=address -g -fno-omit-frame-pointer

SRC = main.cpp request.cpp response.cpp server.cpp

OBJ = $(SRC:%.cpp=%.o)

HEADERS = request.hpp response.hpp server.hpp client.hpp

all : $(NAME)

$(NAME) : $(OBJ) $(HEADERS)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean : 
	-rm -rf $(OBJ)

fclean : clean
	-rm -rf $(NAME)

re : fclean all

b : re
	clear && ./$(NAME)