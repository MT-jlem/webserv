#include "read_config.hpp"

ReadConfig::ReadConfig(int ac, char *av) : _ac(0), _av(0)
{
    _ac = ac;
    _av = av;
}

ReadConfig::~ReadConfig(){
}

void ReadConfig::readingFile()
{
    std::ifstream myfile;
    std::string _configFile;
    std::string newString;
    if (_ac == 2)
    {

        myfile.open(_av);
        if (!myfile.is_open())
        {
            std::cout << "Error: File not found" << std::endl;
            exit (1);
        }
        getline(myfile, _configFile, '\0');
    }
    else
    {
        std::cout << "Wrong arg number\n";
        exit (1);
    }
}


void ReadConfig::checkBraces()
{
    
}

void    ReadConfig::parseServerBlocks()
{
    
}