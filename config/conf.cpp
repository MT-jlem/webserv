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
            isServer = true;
            _serverBlocks[servIndex] = _serverBlocks[servIndex].substr(2, _serverBlocks[servIndex].size()-2);
        }
    }
    else if (keyWord == "server{")
        isServer = true;
}




std::string Conf::parseIp(std::string ip)
{
    int count = std::count(ip.begin(), ip.end(), '.');
    if (count != 3)
    {
        std::cout << "Error: invalid ip address\n";
        exit(1);
    }
    int numInt;
    int j = 0;
    for (int i = 0; i <= 3; i++) // check if the ip is valid 255.255.255.255
    {
        std::stringstream num;
        int pos = ip.find('.', j);
        std::string numStr = ip.substr(j, pos-j);
        num << numStr;
        num >> numInt;
        if (numInt > 255 || numInt < 0 || numStr.size() == 0)
        {
            std::cout << "Error: invalid ip address\n";
            exit(1);
        }
        j = pos+1;
    }
    return ip;
}

bool Conf::listenCharIsValid(const std::string &str)
{
    return str.find_first_not_of("0123456789.:;") == std::string::npos;
}


void	Conf::parseListen(std::string listenValue, std::vector<std::string> &listenDup)
{
	
    if (listenCharIsValid(listenValue) == false)
    {
        std::cout << "Error: invalid listen value\n";
        exit(1);
    }
    if (listenValue[listenValue.size()-1] == ';')
    {
        listenValue = listenValue.substr(0, listenValue.size()-1);
        std::string port;
        std::string ip;
        std::stringstream num;
        int portInt;
        int pos = listenValue.find(':');
        if (pos == -1)
        {
            port = listenValue;
            listenValue = default_ip + ':' + port;
            num << port;
            num >> portInt;
            if (portInt > 1024 && portInt < 65535)
            {
                if (std::count(listenDup.begin(), listenDup.end(), listenValue) == 0)
                {
                    listenDup.push_back(listenValue);
                    listen.push_back(std::make_pair(default_ip, port));
                }
                else
                {
                    std::cout << "Error: duplicate listen value\n";
                    exit(1);
                }
            }
            else
            {
                std::cout << "Error: invalid port number\n";
                exit(1);
            }
        }
        else
        {
            ip = listenValue.substr(0, pos);
            port = listenValue.substr(pos+1, listenValue.size()-pos-1);
            num << port;
            num >> portInt;
            if (portInt > 1024 && portInt < 65535)
            {
                if (std::count(listenDup.begin(), listenDup.end(), listenValue) == 0)
                {
                    listenDup.push_back(listenValue);
                    listen.push_back(std::make_pair(ip, port));
                }
                else
                {
                    std::cout << "Error: duplicate listen value\n";
                    exit(1);
                }
            }
            else
            {
                std::cout << "Error: invalid port number\n";
                exit(1);
            }
        }
    }
    else
    {
        std::cout << "Error: simple directive must end with a semicolon\n";
        exit(1);
    }
}
   

void    Conf::fill_Directives_Locations()
{
    default_ip = "127.0.0.1";
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
    

    listenIndx = 0;
    std::vector<std::string> listenDup;
    for (int i = 0; i < (int)_serverBlocks.size(); i++)
    {
        checkIsServer(i);
        while (isServer == true && (int)_serverBlocks[i].size() > 1)
        {
            std::string key;
            std::string value;
            int pos = _serverBlocks[i].find('\n', 0);
            std::string keyWord = _serverBlocks[i].substr(0, pos);
            _serverBlocks[i] = _serverBlocks[i].substr(pos+1, _serverBlocks[i].size()-pos - 1);
            size_t lindx = keyWord.find_first_not_of(" \n");
            size_t rindx = keyWord.find_last_not_of(" \n");
            if (lindx == std::string::npos || rindx == std::string::npos)
                continue;
            keyWord = keyWord.substr(lindx, rindx+1);
            try
            {
                key = keyWord.substr(0, keyWord.find_first_of(" "));
                keyWord.replace(0, key.size(), "");
                value = keyWord.substr(keyWord.find_first_not_of(" "), keyWord.find_first_of(";}"));
            }
            catch(const std::exception& e)
            {
                // std::cerr << e.what() << '\n';
            }
            
            if (key == "listen")
            {
                parseListen(value, listenDup);
            }
            else if (key == "server_name")
            {
                if (value.find(' ') == std::string::npos)
                {
                    if (value[value.size()-1] == ';')
                    {
                        value = value.substr(0, value.size()-1);
                        // value = value.substr(value.find_first_not_of(" "), value.find_first_of(" ;\n"));
                        serverName = value;
                    }
                    else
                    {
                        std::cout << "Error: simple directive must end with a semicolon\n";
                        exit(1);
                    }
                }
                else
                {
                    std::cout << "Error: server_name accept only one value \n";
                    exit(1);
                }
            }
            else if (key == "index")
            {
                if (value[value.size()-1] == ';')
                {
                    value = value.substr(0, value.size()-1);
                    index = value;
                }
                else
                {
                    std::cout << "Error: simple directive must end with a semicolon\n";
                    exit(1);
                }
            }
            else if (key == "root")
            {
                if (value[value.size()-1] == ';')
                {
                    value = value.substr(0, value.size()-1);
                    root = value;
                }
                else
                {
                    std::cout << "Error: simple directive must end with a semicolon\n";
                    exit(1);
                }
            }
            // else if (key == "max_body_size")
            // {
            //     if (value[value.size()-1] == ';')
            //     {
            //         value = value.substr(0, value.size()-1);
            //         maxBodySize = atoi(value.c_str());
            //     }
            //     else
            //     {
            //         std::cout << "Error: simple directive must end with a semicolon\n";
            //         exit(1);
            //     }
            // }
            // else if (key == "error_page")
            // {
            //     if (value[value.size()-1] == ';')
            //     {
            //         value = value

        }
        // print listen
        for (int i = 0; i < (int)listen.size(); i++)
        {
            std::cout << "listen = " << listen[i].first << ":" << listen[i].second << std::endl;
        }
        std::cout << "--------------------------------\n";
        // print serverName
        std::cout << "serverName = " << serverName << std::endl;
        std::cout << "--------------------------------\n";
        // print root
        std::cout << "root = " << root << std::endl;
        std::cout << "--------------------------------\n";
        // print index
        std::cout << "index = " << index << std::endl;
        std::cout << "--------------------------------\n";
        // print maxBodySize
        std::cout << "maxBodySize = " << maxBodySize << std::endl;
        std::cout << "--------------------------------\n";

    }     
}

// do a check variable to check if a pass a location block pass the curly bracket