NAME = webserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer

SRC =	main.cpp request.cpp response.cpp server.cpp \
		config/conf.cpp config/read_config.cpp

OBJ = $(SRC:%.cpp=%.o)

HEADERS =	request.hpp response.hpp server.hpp \
			config/read_config.hpp config/conf.hpp

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