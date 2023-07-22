NAME = webserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer

SRC = main.cpp request.cpp

OBJ = $(SRC:%.cpp=%.o)

HEADERS = request.hpp

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