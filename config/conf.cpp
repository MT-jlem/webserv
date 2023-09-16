#include "conf.hpp"
#include "../server.hpp"

Conf::Conf(int ac, char *av) : ReadConfig(ac, av)
{

}

Conf::~Conf()
{
}


 
std::string left(const std::string &s)
{
    size_t start = s.find_first_not_of(" \n;");
    return (start == std::string::npos) ? "" : s.substr(start);
}
std::string right(const std::string &s)
{
    size_t end = s.find_last_not_of(" \n;");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string &s) {
    return right(left(s));
}




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


bool checkMaxBodySize(std::string &str)
{
    bool check = true;
    int count = 0;
    if (str[str.size()-2] != 'M' && str[str.size()-2] != 'K')
    {
        count++;
        check = false;
    }
    for (int i = 0; i < (int)str.size() -1; i++)
    {
        if ((str[i] >= 'a' && str[i]<='z') || (str[i] >= 'A' && str[i] <= 'Z'))
            count++;
        else if (str[i] >= '0' && str[i] <= '9')
            continue;
        else
        {
            check = false;
        }
        if (count == 2)
            check = false;
    }
    return check;
}


void    Conf::parsMaxBodySize(std::string &str)
{
    if (str[str.size()-1] == ';')
    {
        if (checkMaxBodySize(str) == true)
        {
            str = str.substr(0, str.size()-1);
            maxBodySize = atoi(str.c_str());
        }
        else
        {
            std::cout << "Megabytes, M, and Kilobytes, K, are the accepted units \
in client_max_body_size.\n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: simple directive must end with a semicolon\n";
        exit(1);
    }
}


void    Conf::parsServerName(std::string value)
{
    if (value.find(' ') == std::string::npos)
    {
        if (value[value.size()-1] == ';')
        {
            value = value.substr(0, value.size()-1);
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



void    Conf::parsRootIndex(std::string value, std::string key)
{
    if (key == "index")
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
}

bool    checkErrorPagesCode(const std::string key)
{ 
    // 100 101 102 200 201 202 203 204 205 206 207 300 301 302 303 304 305 307 308 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 418 419 420 421 422 423 424 428 429 431 451 500 501 502 503 504 505 507 511
    if (key == "100" || key == "101" || key == "102" || key == "200" || key == "201" || key == "511" \
         || key == "202" || key == "203" || key == "204" || key == "205" || key == "206" \
         || key == "207" || key == "300" || key == "301" || key == "302" || key == "303" \
         || key == "304" || key == "305" || key == "307" || key == "308" || key == "400" \
         || key == "401" || key == "402" || key == "403" || key == "404" || key == "405" \
         || key == "406" || key == "407" || key == "408" || key == "409" || key == "410" \
         || key == "411" || key == "412" || key == "413" || key == "414" || key == "415" \
         || key == "416" || key == "417" || key == "418" || key == "419" || key == "420" \
         || key == "421" || key == "422" || key == "423" || key == "424" || key == "428" \
         || key == "429" || key == "431" || key == "451" || key == "500" || key == "501" \
         || key == "502" || key == "503" || key == "504" || key == "505" || key == "507")
        return true;
    return false;
}

void    Conf::parsError_page(std::string value, bool check)
{
    std::vector<std::string> val;
    bool checkKey = false;
    if (!check)
    {
        if (value[value.size()-1] == ';')
        {
            value = value.substr(0, value.size()-1);
            for (int i = 0; i < (int)value.size(); i++)
            {
                int pos = value.find(' ');
                value = trim(value);
                if (pos == -1)
                {
                    val.push_back(value);
                    break;
                }
                else
                {
                    std::string key = value.substr(0, pos);

                    value = value.substr(pos, value.size());
                    key = trim(key);
                    value = trim(value);
                    if (checkErrorPagesCode(key) == true)
                    {
                        checkKey = true;
                        val.push_back(key);
                    }
                    else
                    {
                        std::cout << "Error: invalid error_page key "<< key << std::endl;
                        exit(1);
                    }
                }
            }
        }
        else
        {
            std::cout << "Error: simple directive must end with a semicolon\n";
            exit(1);
        }
        for (int j = 0; j < (int)val.size()-1; j++)
        {
            errorPage[val[j]] = val[val.size()-1];
        }
    }


}

void    Conf::parsLocation(std::string value)
{
    // there is two cases for location block lac /{ and loc / \n {
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
        // std::cout << _serverBlocks[i] << std::endl;
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
                parseListen(value, listenDup);
            else if (key == "server_name")
                parsServerName(value);
            else if (key == "index" || key == "root")
                parsRootIndex(value, key);
            else if (key == "client_max_body_size")
                parsMaxBodySize(value);
            else if (key == "error_page")
                parsError_page(value, false);
            else if (key == "location")
                parsLocation(value);
            
            std::cout << key << "<-->" << value << std::endl;
            
        }








        // // print listen
        // for (int i = 0; i < (int)listen.size(); i++)
        // {
        //     std::cout << "listen = " << listen[i].first << ":" << listen[i].second << std::endl;
        // }
        // std::cout << "--------------------------------\n";
        // // print serverName
        // std::cout << "serverName = " << serverName << std::endl;
        // std::cout << "--------------------------------\n";
        // // print root
        // std::cout << "root = " << root << std::endl;
        // std::cout << "--------------------------------\n";
        // // print index
        // std::cout << "index = " << index << std::endl;
        // std::cout << "--------------------------------\n";
        // // print maxBodySize
        // std::cout << "maxBodySize = " << maxBodySize << std::endl;
        // std::cout << "--------------------------------\n";
        // // print errorPage
        // std::map<std::string, std::string>::iterator it;
        // for (it = errorPage.begin(); it != errorPage.end(); it++)
        // {
        //     std::cout << "errorPage = " << it->first << " " << it->second << std::endl;
        // }
        // std::cout << "--------------------------------\n";

    }     
}

// do a check variable to check if a pass a location block pass the curly bracket