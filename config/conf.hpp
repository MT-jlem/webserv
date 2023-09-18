#ifndef CONF_HPP
#define CONF_HPP


#include <map>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>
#include <vector>
// #include <utility
#include "read_config.hpp"
#include "../server.hpp"




#define GET 0
#define POST 1
#define DELETE 2

// #define BUFFER_SIZE 8388608




// #define GET 0
// #define POST 1
// #define DELETE 2
// class ServerLocation {
//     protected:
//     public:
// 		std::string path;
// 		std::string root;
// 		std::string index;
// 		bool autoIndex;
// 		int methods[3];
// 		std::pair<std::string, std::string> redir;
// 		bool cgi;
// 		std::map<std::string, std::string> errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
// 		std::string upload;
// 		std::map<std::string, std::string> cgiPath;
// 		ServerLocation(){};
//     	~ServerLocation(){};
// };

// class Location {
// private:
// public:
// 	std::string path;
// 	std::string root;
// 	std::string index;
// 	bool autoIndex;
// 	int methods[3]; // GET 0 POST 1 DELETE 2
// 	std::pair<std::string, std::string> redir; // return
// 	std::map<std::string, std::string> errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
// 	std::string upload; //
// 	std::map<std::string, std::string> cgiPath;
// 	Location();
// 	~Location(){};

// };

// class Server{
// private:
// public:
// 	std::vector<std::pair<std::string, std::string> > listen;
// 	std::string root;
// 	std::string index;
// 	std::string serverName;
// 	size_t		maxBodySize;
// 	std::map<std::string, std::string> errorPage;
// 	Location servLoc;
// 	std::vector<Location> loc;


// 	// std::string root;
// 	// std::string index;
// 	// std::set<std::string> serverName;
// 	// std::vector<std::pair<std::string, std::string> > listen;
// 	// size_t maxBodySize;
// 	// std::map<std::string, std::string> errorPage; //maybe a vector
// 	// std::vector<Location> loc;
// 	//vector  fd;
// 	// Server();
// 	// ~Server();
// };

class Conf : public ReadConfig
{
    private:
		bool isLocation;
		int listenIndx;
		std::string default_ip;
    public:
		Server singleServer;
		std::vector<Server> servers;
        Conf(int ac, char *av);
        ~Conf();
        void    		fill_Directives_Locations();
        void    		checkIsServer(int serverIndex);
		void			parseListen(std::string listen, std::vector<std::string> &listenDup);
		std::string		parseIp(std::string ip);
		bool 			listenCharIsValid(const std::string &str);
		void			parsMaxBodySize(std::string &str);
		void    		parsServerName(std::string value);
		void    		parsRootIndex(std::string value, std::string key);
		void			parsError_page(std::string value);
		void			parsLocation(std::string key, std::string value);
		void			parsAutoindex(std::string value);
		void			parsReturn(std::string value);
		void			parsMethods(std::string value);
		void			parsCgi(std::string value);
		//parse();
};

#endif