#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <utility>

class Location {
private:
public:
	std::string root;
	std::string index;
	bool autoIndex;
	std::vector<std::string> methods;
	std::pair<std::string, std::string> returnn; 
	//cgi
	std::string upload;
	std::string path;
	Location();
	~Location();

};

class Server{
private:
public:
	std::string root;
	std::string index;
	size_t maxBodySize;
	//listen
	//host
	//server name
	std::pair<std::string, std::string> errorPage;
	std::vector<Location> loc;
	Server();
	~Server();
};


#endif