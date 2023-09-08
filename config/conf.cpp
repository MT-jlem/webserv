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




void    Conf::checkIsServer(int servIndex)
{
    isServer = false;
    int pos = _serverBlocks[servIndex].find('\n', 0);
    std::string keyWord = _serverBlocks[servIndex].substr(0, pos);
    _serverBlocks[servIndex] = _serverBlocks[servIndex].substr(pos+1, _serverBlocks[servIndex].size()-pos - 1);
    keyWord.erase(remove(keyWord.begin(), keyWord.end(), ' '), keyWord.end());
    if (keyWord == "server")
    {
        if (_serverBlocks[servIndex][0] == '{')
        {
            puts("hnaaaa12");
            isServer = true;
            _serverBlocks[servIndex] = _serverBlocks[servIndex].substr(2, _serverBlocks[servIndex].size()-2);
            // std::cout << "=>" << _serverBlocks[servIndex] << std::endl;
        }
    }
    else if (keyWord == "server{")
    {
        isServer = true;
        // std::cout << "=>" << _serverBlocks[servIndex] << std::endl;
    }
}




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
    
    // std::cout << this->_serverBlocks[0] << std::endl;
    // std::cout << "--------------------------------\n";
    // std::cout << this->_serverBlocks[1] << std::endl;
    listenIndx = 0;
    for (int i = 0; i < (int)_serverBlocks.size(); i++)
    {
        checkIsServer(i);
        while (isServer == true && (int)_serverBlocks[i].size() > 1)
        {
            int pos = _serverBlocks[i].find('\n', 0);
            std::string keyWord = _serverBlocks[i].substr(0, pos);
            _serverBlocks[i] = _serverBlocks[i].substr(pos+1, _serverBlocks[i].size()-pos - 1);
            if (keyWord != "\n")
            {
                size_t lindx = keyWord.find_first_not_of(" \n");
                size_t rindx = keyWord.find_last_not_of(" \n");
                if (lindx == std::string::npos || rindx == std::string::npos)
                    continue;
                keyWord = keyWord.substr(lindx, rindx+1);
            }
            std::string key = keyWord.substr(0, keyWord.find_first_of(" "));
            std::string value = keyWord.substr(keyWord.find_first_of(" ")+1, keyWord.find_first_of(";}"));
            if (value == "" || value == "")
                std::cout << key << " value is empty" << std::endl;
            // if (key == "listen")
            // {
            //     listen.push_back(std::make_pair(value, "80"));
            // }

            // std::cout << /* "=>" << keyWord <<  " <=====> "*/  key << " <=====> " << value << std::endl;
        }
        std::cout << "--------------------------------\n";
    }     
}