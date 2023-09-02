#ifndef READ_CONFIG_HPP
#define READ_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


class read_config
{
private:
    std::string     _config_file;
    int             _ac;
    char            *_av;
public:
    read_config(int ac, char *av);
    ~read_config();

    std::string readingFile();
};




#endif