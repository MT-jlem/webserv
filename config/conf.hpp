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

#define GET 0
#define POST 1
#define DELETE 2
class ServerLocation {
    protected:
		std::string path;
		std::string root;
		std::string index;
		bool autoIndex;
		int methods[3];
		std::pair<std::string, std::string> redir;
		bool cgi;
		std::map<std::string, std::string> errorPage; // we can use "vector" instead of "set" && and use "int" instead of "string"
		std::string upload;
		std::map<std::string, std::string> cgiPath;
    public:
		ServerLocation(){};
    	~ServerLocation(){};
};

class Conf : public ReadConfig
{
    private:
		int listenIndx;
		std::string default_ip;
    public:
	    std::vector<std::pair<std::string, std::string> > listen;
        std::string root;
	    std::string index;
	    std::string serverName;
	    size_t		maxBodySize;
	    std::map<std::string, std::string> errorPage;
	    std::vector<ServerLocation> loc;

        Conf(int ac, char *av);
        ~Conf();
        void    	fill_Directives_Locations();
        void    	checkIsServer(int serverIndex);
		void		parseListen(std::string listen, std::vector<std::string> &listenDup);
		std::string	parseIp(std::string ip);
		bool 		listenCharIsValid(const std::string &str);
		void		parsMaxBodySize(std::string &str);
		void    	parsServerName(std::string value);
		void    	parsRootIndex(std::string value, std::string key);
		void		parsError_page(std::string value, bool check);
		void		parsLocation(std::string value);


};

#endif