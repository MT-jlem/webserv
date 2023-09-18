#ifndef READ_CONFIG_HPP
#define READ_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>

class ReadConfig
{
    protected:
        std::string     _configFile;
        int             _ac;
        char            *_av;
        std::vector<std::string> _serverBlocks;
        bool isServer;
    public:
        ReadConfig(int ac, char *av);
        ~ReadConfig();

        void readingFile(); // read the config file and return a string
        void checkBraces(); // check if the braces are well placed
};


#endif