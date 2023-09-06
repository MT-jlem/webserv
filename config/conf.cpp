#include "conf.hpp"
#include "../server.hpp"

Conf::Conf(int ac, char *av) : ReadConfig(ac, av)
{
}

Conf::~Conf()
{
}


// void Conf::readCo()
// {
//     // print the vector of server blocks
    // std::cout << "size of _serverBlocks = " << this->_serverBlocks.size() << std::endl;
    // for(int i = 0; i < (int)this->_serverBlocks.size(); i++) 
    // {
    //     std::cout << "-------------------------------------\n";
    //         std::cout << this->_serverBlocks[i] << std::endl;
    // }
    // for(int i = 0; i < (int)this->_serverBlocks.size(); i++)
    // {

    // }
    
//     // print the vector of server blocks
// }




void    Conf::fill_Directives_Locations()
{
    for (int i = 0; i < (int)_serverBlocks.size(); i++)
    {
        size_t lindx = _serverBlocks[i].find_first_not_of(" \n\r\t\f\v");
        size_t rindx = _serverBlocks[i].find_last_not_of(" \n\r\t\f\v");
        _serverBlocks[i] = _serverBlocks[i].substr(lindx, rindx+1);
        for (int j = 0; j < (int)_serverBlocks[i].size(); j++)
        {
            if (_serverBlocks[i][j] == '#')
            {
                int pos = _serverBlocks[i].find('\n', j);
                _serverBlocks[i] = _serverBlocks[i].erase(j, pos-j);
            }
            if (_serverBlocks[i][j] == ';' || _serverBlocks[i][j] == '{')
                if (_serverBlocks[i][j+1] != '\n')
                    _serverBlocks[i] = _serverBlocks[i].insert(j+1, "\n");
        }

    }
    std::cout << this->_serverBlocks[0] << std::endl;
    std::cout << "--------------------------------\n";
    std::cout << this->_serverBlocks[1] << std::endl;

    // std::cout << pos << "\n";
    std::cout << "--------------------------------\n";
    int indx = 0;
    for (int i = 0; i < (int)_serverBlocks.size(); i++) // making server as a singel string without new line '\n'
    {
        while ((int)_serverBlocks[i].size() > 1)
        {
            int pos = _serverBlocks[i].find('\n', 0);
            std::string keyWord = _serverBlocks[i].substr(0, pos);
            _serverBlocks[i] = _serverBlocks[i].substr(pos+1, _serverBlocks[i].size()-pos - 1);
            std::cout << keyWord << "\n";
            // j+=pos;
            // std::cout << "j = " << j << "\n";
            std::cout << "size = " << _serverBlocks[i].size() << "\n";
        }

        // while (_serverBlocks[i].size())
        // {
        // }
        
        // int pos = _serverBlocks[i].find(' ');
        // // std::cout << pos << "\n";
        // std::string keyWord = _serverBlocks[i].substr(indx, pos);
    }     
}