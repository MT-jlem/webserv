#ifndef READ_CONFIG_HPP
#define READ_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


class ReadConfig
{
    private:
        std::string     _configFile;
        int             _ac;
        char            *_av;
        std::vector<std::vector<std::string> > _serverBlocks;
    public:
        ReadConfig(int ac, char *av);
        ~ReadConfig();

        void readingFile(); // read the config file and return a string
        void checkBraces(); // check if the braces are well placed
        void parseServerBlocks(); // parse the string and return a vector of server blocks
};


#endif