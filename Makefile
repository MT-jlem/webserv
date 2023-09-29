NAME = webserv

CC = c++

CPPFLAGS = -Wall -Wextra -Werror  -std=c++98 -fsanitize=address -fno-omit-frame-pointer

SRC =	main.cpp request.cpp server.cpp response.cpp \
		config/conf.cpp config/read_config.cpp client.cpp

OBJ = $(SRC:%.cpp=%.o)

HEADERS =	config/read_config.hpp config/conf.hpp client.hpp \
			request.hpp response.hpp server.hpp

all : $(NAME)

$(NAME) : $(OBJ) $(HEADERS)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp $(HEADERS)
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean : 
	-rm -rf $(OBJ)

fclean : clean
	-rm -rf $(NAME)

re : fclean all
