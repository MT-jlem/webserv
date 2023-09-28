#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <netdb.h>
#include <string>
#include <sys/poll.h>
#include <vector>
#include <utility>
#include <set>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#define GET 0
#define POST 1
#define DELETE 2

#define BUFFER_SIZE 1024

class Location {
private:
public:
	std::string path;
	std::string root;
	std::string index;
	bool autoIndex;
	bool trackAutoIndex;
	int methods[3]; 								// GET 0 POST 1 DELETE 2 done
	std::pair<std::string, std::string> redir; 	   // return 				 done
	std::map<std::string, std::string> errorPage; // 						 done
	std::string upload; 						 // 						 not yet
	std::map<std::string, std::string> cgiPath; // 							 done
	Location();
	~Location();

};

class Server{
private:
public:
//conf part
	std::vector<std::pair<std::string, std::string> > _listen;
	std::string root;
	std::string index;
	std::string serverName;
	long long		maxBodySize;
	std::map<std::string, std::string> errorPage;
	Location servLoc;
	std::vector<Location> loc;
//creation part
    struct addrinfo *servInfo;
	std::vector<pollfd> fds;
    struct addrinfo hints;
    std::vector<int>    fd;
	Server();
	~Server();
    int createServer();
	void closeFd();
};


#endif