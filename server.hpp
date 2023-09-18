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

#define BUFFER_SIZE 8388608

// class Location {
// private:
// public:
// 	std::string path;
// 	std::string root;
// 	std::string index;
// 	bool autoIndex;
// 	int methods[3];
// 	std::pair<std::string, std::string> redir;
// 	bool cgi;
// 	std::map<std::string, std::string> errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
// 	std::string upload;
// 	std::map<std::string, std::string> cgiPath;
// 	Location();
// 	~Location();

// };

// class Server{
// private:
// public:
// 	std::string root;
// 	std::string index;
// 	std::set<std::string> serverName;
// 	std::vector<std::pair<std::string, std::string> > listen;
// 	size_t maxBodySize;
// 	std::map<std::string, std::string> errorPage; //maybe a vector
// 	std::vector<Location> loc;
// 	//vector  fd;
// 	Server();
// 	~Server();
// };

class Location {
private:
public:
	std::string path;
	std::string root;
	std::string index;
	bool autoIndex;
	int methods[3]; // GET 0 POST 1 DELETE 2
	std::pair<std::string, std::string> redir; // return
	std::map<std::string, std::string> errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
	std::string upload; //
	std::map<std::string, std::string> cgiPath;
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
	size_t		maxBodySize;
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
    void createServer();
};


#endif