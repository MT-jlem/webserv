#ifndef CONF_HPP
#define CONF_HPP


#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>
// #include <utility
#include "read_config.hpp"

#define GET 0
#define POST 1
#define DELETE 2
class ServerLocation {
    protected:
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
    	ServerLocation(){};
    	~ServerLocation(){};
};

class Conf : public ReadConfig
{
    private:
		int listenIndx;
    public:
        std::string root;
	    std::string index;
	    std::set<std::string> serverName;
	    std::vector<std::pair<std::string, std::string> > listen;
	    size_t maxBodySize;
	    std::pair<std::string, std::set<std::string> > errorPage; //maybe a vector
	    std::vector<ServerLocation> loc;

        Conf(int ac, char *av);
        ~Conf();
        void    fill_Directives_Locations();
        void    checkIsServer(int serverIndex);
		void	parseListen(std::string listen);
};

#endif