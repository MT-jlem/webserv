#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <poll.h>

#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sstream>

#define GET 0
#define POST 1
#define DELETE 2
class Location {
private:
public:
	std::string path;
	std::string root;
	std::string index;
	bool autoIndex;
	int methods[3];
	std::pair<std::string, std::string> redir;
	//cgi
	std::pair<std::string, std::set<std::string> > errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
	std::string upload;
	Location();
	~Location();

};

class server{
private:
public:
        int fd_socket;
        socklen_t adresslen;
        struct sockaddr_in client_address;
        long count_read;
        struct sockaddr_in serv_addr;
        //  std::vector<pollfd> fd;
        server(int port,std::vector<pollfd> &fd,std::vector<int> &server_fd);
        const int   &get_fd_srv();
        int     create_socket();
        void    binding_socket(int fd_socket);
        void    listening_socket(int fd_socket);
        int     accepte_connect(int fd_socket);
        void    get_info_server();
		std::string root;
		std::string index;
		std::set<std::string> serverName;
		std::vector<std::pair<std::string, std::string> > host;
		size_t maxBodySize;
		std::pair<std::string, std::set<std::string> > errorPage; //maybe a vector
		std::vector<Location> loc;
		//vector  fd;
		server();
		~server();
};


#endif