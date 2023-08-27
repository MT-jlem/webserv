#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <utility>
#include <set>

class Location {
private:
public:
	std::string path;
	std::string root;
	std::string index;
	bool autoIndex;
	std::vector<std::string> methods;
	std::pair<std::string, std::string> redir;
	//cgi
	std::pair<std::string, std::set<std::string> > errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
	std::string upload;
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
	std::pair<std::string, std::set<std::string> > errorPage; //must be a vector
	std::vector<Location> loc;
	Server();
	~Server();
};


#endif