#include "read_config.hpp"

read_config::read_config(int ac, char *av) : _ac(0), _av(0)
{
    _ac = ac;
    _av = av;
}

read_config::~read_config()
{
}

std::string read_config::readingFile()
{
    std::ifstream myfile;
    std::string fileContent;
    std::string newString;
    if (_ac == 2)
    {

        myfile.open(_av);
        if (!myfile.is_open())
        {
            std::cout << "Error: File not found" << std::endl;
            exit (1);
        }
        getline(myfile, fileContent, '\0');
    }
    else
    {
        std::cout << "Wrong arg number\n";
        exit (1);
    }
    return fileContent;
}
