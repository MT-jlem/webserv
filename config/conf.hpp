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
#include "read_config.hpp"
#include "../server.hpp"



#define GET 0
#define POST 1
#define DELETE 2

class Conf : public ReadConfig
{
    private:
		bool isLocation;
		int listenIndx;
		std::string default_ip;
	public:
		std::vector<std::string> listenDup;
		Server singleServer;
		std::vector<Server> servers;
        Conf(int ac, char *av);
        ~Conf();
        void    		fill_Directives_Locations();
        void    		checkIsServer(int serverIndex);
		void			parseListen(std::string listen);
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
		void			parsUpload(std::string value);
		//parse();
};

#endif