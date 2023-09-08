#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <map>

#define GET 0
#define POST 1
#define DELETE 2

#define BUFFER_SIZE 32664

class Location {
private:
public:
	std::string path;
	std::string root;
	std::string index;
	bool autoIndex;
	int methods[3];
	std::pair<std::string, std::string> redir;
	bool cgi;
	std::pair<std::string, std::set<std::string> > errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
	std::string upload;
	std::map<std::string, std::string> cgiPath;
	Location();
	~Location();

};

class Server{
private:
public:
	std::string root;
	std::string index;
	std::set<std::string> serverName;
	std::vector<std::pair<std::string, std::string> > listen;
	size_t maxBodySize;
	std::pair<std::string, std::set<std::string> > errorPage; //maybe a vector
	std::vector<Location> loc;
	//vector  fd;
	Server();
	~Server();
};


#endif