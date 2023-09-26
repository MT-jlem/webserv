NAME = webserv

CC = c++

CPPFLAGS =  -std=c++98 -fsanitize=address -fno-omit-frame-pointer

SRC =	main2.cpp request.cpp server.cpp response.cpp \
		config/conf.cpp config/read_config.cpp

OBJ = $(SRC:%.cpp=%.o)

HEADERS =	config/read_config.hpp config/conf.hpp

all : $(NAME)

$(NAME) : $(OBJ) $(HEADERS)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean : 
	-rm -rf $(OBJ)

fclean : clean
	-rm -rf $(NAME)

re : fclean all

b : re clean
	clear && ./$(NAME) .conf
