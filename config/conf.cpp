#include "conf.hpp"
#include <cstdlib>
#include <iostream>

Conf::Conf(int ac, char *av) : ReadConfig(ac, av)
{
    isLocation = false;
    listenIndx = 0;
    default_ip = "127.0.0.1";
}

Conf::~Conf()
{
}



std::string left(const std::string &s, std::string str)
{
    size_t start = s.find_first_not_of(str);
    return (start == std::string::npos) ? "" : s.substr(start);
}
std::string right(const std::string &s, std::string str)
{
    size_t end = s.find_last_not_of(str);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string &s, std::string str) {
    return right(left(s, str), str);
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
    else
    {
        std::cout << "Error: invalid server block\n";
        exit(1);
    }
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
    for (int i = 0; i <= 3; i++)
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
    return str.find_first_not_of("0123456789.:") == std::string::npos;
}

void	Conf::parseListen(std::string listenValue)
{
	
    if (listenValue[listenValue.size()-1] == ';')
    {
        listenValue = trim(listenValue.substr(0, listenValue.size()-1), " ");
        if (listenCharIsValid(listenValue) == false)
        {
            std::cout << "Error: invalid listen value\n";
            exit(1);
        }
        std::string port;
        std::string ip;
        std::stringstream num;
        int portInt;
        int pos = listenValue.find(':');
        if (pos == -1)
        {
            if (!(listenValue.find_first_not_of("0123456789") == std::string::npos))
            {
                std::cout << "Error: invalid port number\n";
                exit(1);
            }
            port = listenValue;
            listenValue = default_ip + ':' + port;
            num << port;
            num >> portInt;
            if (portInt > 1024 && portInt < 65535)
            {
                listenDup.push_back(listenValue);
                singleServer._listens.push_back(listenValue);
                singleServer._listen.push_back(std::make_pair(default_ip, port));
            }
            else
            {
                std::cout << "Error: invalid port number\n";
                exit(1);
            }
        }
        else
        {
            if (listenValue[0] == ':')
            {
                std::cout << "Error: invalid ip address\n";
                exit(1);
            }
            ip = parseIp(listenValue.substr(0, pos));
            port = listenValue.substr(pos+1, listenValue.size()-pos-1);
            num << port;
            num >> portInt;
            if (portInt > 1024 && portInt < 65535)
            {

                listenDup.push_back(listenValue);
                singleServer._listens.push_back(listenValue);
                singleServer._listen.push_back(std::make_pair(ip, port));
            }
            else
            {
                std::cout << "Error: invalid port number\n";
                exit(1);
            }
        }
        if (std::count(singleServer._listens.begin(), singleServer._listens.end(), listenValue) > 1)
        {
            std::cout << "Error: duplicate listen directive\n";
            exit(1);
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
    bool checkRet = true;
    if (str.find(' ') == std::string::npos)
    {
        if (tolower(str[str.size()-1]) == 'm' || tolower(str[str.size()-1]) == 'k' || isdigit(str[str.size()-1]))
        {
            for (int i = 0; i < (int)str.size() - 1; i++)
            {
                if (!isdigit(str[i]))
                    return false;
            }
        }
        else
            return false;
     }
    else
    {
        std::cout << "Error: server_name accept only one value \n";
        exit(1);
    }
    return checkRet;
}


void    Conf::parsMaxBodySize(std::string &str)
{
    if (str[str.size()-1] == ';')
    {
        str = trim(str.substr(0, str.size()-1), " ");
        if (str.size() == 1 && !isdigit(str[0]))
        {
            std::cout << "Error: invalid client_max_body_size value\n";
            exit(1);
        }
        if (checkMaxBodySize(str) == true)
        {
            singleServer.maxBodySize = atoi(str.c_str());
            if (tolower(str[str.size()-1]) == 'k')
                singleServer.maxBodySize *= 1000;
            else if (tolower(str[str.size()-1]) == 'm')
                singleServer.maxBodySize *= 1000000;
        }
        else
        {
            std::cout << "Error: invalid client_max_body_size value\n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: max_body_size directive must end with a semicolon\n";
        exit(1);
    }
}


void    Conf::parsServerName(std::string value)
{
    if (value[value.size()-1] == ';')
        value = trim(value.substr(0, value.size()-1), " ");
    else
    {
        std::cout << "Error: server name directive must end with a semicolon\n";
        exit(1);
    }
    if (value.find(' ') == std::string::npos)
        singleServer.serverName = value;
    else
    {
        std::cout << "Error: server_name accept only one value \n";
        exit(1);
    }
}



void    Conf::parsRootIndex(std::string value, std::string key)
{
    if (isLocation == false)
    {
        if (key == "index")
        {
            if (value[value.size()-1] == ';')
            {
                value = trim(value.substr(0, value.size()-1), " ");
                if (value.find(' ') == std::string::npos)
                    singleServer.index = value;
                else
                {
                    std::cout << "Error: index accept only one value \n";
                    exit(1);
                }
                return;
            }
            else {
                std::cout << "Error: index directive must end with a semicolon\n";
                exit(1);
            }
        }
        else if (key == "root")
        {
            if (value[value.size()-1] == ';')
            {
                value = trim(value.substr(0, value.size()-1), " ");
                if (value.find(' ') == std::string::npos)
                    singleServer.root = value;
                else
                {
                    std::cout << "Error: root accept only one value \n";
                    exit(1);
                }
                return;
            }
            else 
            {
                std::cout << "Error: root directive must end with a semicolon\n";
                exit(1);
            }
        }
    }
    else
    {
        if (key == "index")
        {
            if (value[value.size()-1] == ';')
            {
                value = trim(value.substr(0, value.size()-1), " ");
                if (value.find(' ') == std::string::npos)
                    singleServer.servLoc.index = value;
                else
                {
                    std::cout << "Error: index accept only one value \n";
                    exit(1);
                }
                return;
            }
            else 
            {
                std::cout << "Error: index directive must end with a semicolon\n";
                exit(1);
            }
        }
        else if (key == "root")
        {
            if (value[value.size()-1] == ';')
            {
                value = trim(value.substr(0, value.size()-1), " ");
                if (value.find(' ') == std::string::npos)
                    singleServer.servLoc.root = value;
                else
                {
                    std::cout << "Error: root accept only one value \n";
                    exit(1);
                }
                return;
            }
            else 
            {
                std::cout << "Error: root directive must end with a semicolon\n";
                exit(1);
            }
        }
    }
}

bool    checkErrorPagesCode(const std::string key)
{ 
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

void    Conf::parsError_page(std::string value)
{
    std::vector<std::string> val;
    bool checkKey = false;
    if (value[value.size()-1] == ';')
    {
        value = value.substr(0, value.size()-1);
        for (int i = 0; i < (int)value.size(); i++)
        {
            int pos = value.find(' ');
            value = trim(value, " \n;");
            if (pos == -1)
            {
                val.push_back(value);
                break;
            }
            else
            {
                std::string key = value.substr(0, pos);
                value = value.substr(pos, value.size());
                key = trim(key, " \n;");
                value = trim(value, " \n;");
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
        std::cout << "Error: error page directive must end with a semicolon\n";
        exit(1);
    }
    for (int j = 0; !isLocation && j < (int)val.size()-1; j++)
    {
        singleServer.errorPage[val[j]] = val[val.size()-1];
    }
    for (int j = 0; isLocation && j < (int)val.size()-1; j++)
    {
        singleServer.servLoc.errorPage[val[j]] = val[val.size()-1];
    }
}

void   Conf::parsAutoindex(std::string value)
{
    if (value[value.size()-1] == ';')
    {
        singleServer.servLoc.trackAutoIndex = true;
        value = trim(value.substr(0, value.size()-1), " ");
        if (value == "on")
            singleServer.servLoc.autoIndex = true;
        else if (value == "off")
            singleServer.servLoc.autoIndex = false;
        else
        {
            std::cout << "Error: invalid autoindex value\n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: autoindex directive must end with a semicolon\n";
        exit(1);
    }
}

    
void    Conf::parsReturn(std::string value)
{
    if (value[value.size()-1] == ';')
    {
        value = value.substr(0, value.size()-1);
        int pos = value.find(' ');
        if (pos != -1)
        {
            std::string key = value.substr(0, pos);
            value = value.substr(pos, value.size());
            key = trim(key, " \n;");
            value = trim(value, " \n;");
            singleServer.servLoc.redir = std::make_pair(key, value);
        }
        else
        {
            std::cout << "Error: invalid return value\n";
            exit(1);
        }
    }
    else 
    {
        std::cout << "Error: return directive must end with a semicolon\n";
        exit(1);
    }
}

void    Conf::parsMethods(std::string value)
{
    if (value[value.size()-1] == ';')
    {
        value = value.substr(0, value.size()-1);
        if (value.size())
        {
            while (value.size())
            {
                int pos = value.find(' ');
                if (pos == -1)
                    pos = value.size();
                std::string key = value.substr(0, pos);
                value = value.substr(pos, value.size());
                key = trim(key, " \n;");
                value = trim(value, " \n;");
                if (key == "GET")
                    singleServer.servLoc.methods[0] = 1;
                else if (key == "POST")
                    singleServer.servLoc.methods[1] = 1;
                else if (key == "DELETE")
                    singleServer.servLoc.methods[2] = 1;
                else
                {
                    std::cout << key << " Error: invalid methods value\n";
                    exit(1);
                }
            }
        }
        else
        {
            std::cout << "Error: invalid methods value\n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: allowed_methods directive must end with a semicolon\n";
        exit(1);
    }
}
void	Conf::parsCgi(std::string value)
{
    if (value[value.size()-1] == ';')
    {
        value = trim(value.substr(0, value.size()-1), " ");
        int pos = value.find(' ');
        if (pos != -1)
        {
            std::string key = value.substr(0, pos);
            value = value.substr(pos, value.size());
            key = trim(key, " \n;");
            value = trim(value, " \n;");
            if (value.find(" ") != std::string::npos)
            {
                std::cout << "Error: invalid cgi_pass value\n";
                exit(1);
            }
            singleServer.servLoc.cgiPath[key] = value;
        }
        else
        {
            std::cout << "Error: invalid cgi_pass value\n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: cgi_pass directive must end with a semicolon\n";
        exit(1);
    }
}


void    Conf::parsUpload(std::string value)
{

    if (value[value.size()-1] == ';')
    {
        value = trim(value.substr(0, value.size()-1), " ");
        if (value.find(' ') == std::string::npos)
            singleServer.servLoc.upload = value;
        else
        {
            std::cout << "Error: upload accept only one value \n";
            exit(1);
        }
    }
    else
    {
        std::cout << "Error: upload directive must end with a semicolon\n";
        exit(1);
    }
}


void    Conf::parsLocation(std::string key, std::string value)
{
    if (key == "root" || key == "index")
    {
        if (key == "root")
        {
            if (singleServer.servLoc.root.size() == 0)
                parsRootIndex(value, key);
            else
            {
                std::cout << "Error: duplicate root directive\n";
                exit(1);
            }
        }
        if (key == "index")
        {
            if (singleServer.servLoc.index.size() == 0)
                parsRootIndex(value, key);
            else
            {
                std::cout << "Error: duplicate index directive\n";
                exit(1);
            }
        }
    }
    else if (key == "upload")
    {
        if (singleServer.servLoc.upload.size() == 0)
            parsUpload(value);
        else
        {
            std::cout << "Error: duplicate upload directive\n";
            exit(1);
        }
    }
    else if (key == "autoindex")
    {
        if (!singleServer.servLoc.trackAutoIndex)
            parsAutoindex(value);
        else
        {
            std::cout << "Error: duplicate autoindex directive\n";
            exit(1);
        }
    }
    else if (key == "error_page")
        parsError_page(value);
    else if (key == "return")
        parsReturn(value);
    else if (key == "allowed_methods")
        parsMethods(value);
    else if (key == "cgi_pass")
        parsCgi(value);
    else
    {
        std::cout << "Error: invalid directive\n";
        exit(1);
    }
}

void    Conf::fill_Directives_Locations()
{
    std::string serv;

    for (int i = 0; i < (int)_serverBlocks.size(); i++)
    {
        std::string serv_push;
        _serverBlocks[i] = trim(_serverBlocks[i], " \n");
        int indx = 0;
        while (indx <= (int)_serverBlocks[i].size())
        {
            if (_serverBlocks[i][indx] == '#')
            {
                indx = _serverBlocks[i].find('\n', indx);
            }
            else if (_serverBlocks[i][indx] == '{' || _serverBlocks[i][indx] == '}')
            {
                serv += '\n';
                serv += _serverBlocks[i][indx];
                serv += '\n';
            }
            else if (_serverBlocks[i][indx] == ';')
            {
                serv += _serverBlocks[i][indx];
                serv += '\n';
                serv = trim(serv, " ");
                
            }
            else if (_serverBlocks[i][indx] == '\n')
            {
                indx++;
                continue;
            }
                
            else
                serv += _serverBlocks[i][indx];

            serv_push += serv;
            serv = ""; 
            indx++;
        }
        _serverBlocks[i] = serv_push;
    }

    for (int i = 0; i < (int)_serverBlocks.size(); i++)
    {
        int checkLocationBrace = 0;
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
                value = trim(value, " ");
                if (value[value.size() - 1] == ';' && value.size() == 1)
                {
                    std::cout << "Error: no value for " << key << " directive\n";
                    exit(1);
                }
            }
            catch(const std::exception& e)
            {
                if (key == "location")
                {
                    std::cout << "Error: No path for location\n";
                    exit(1);
                }
            }
            if (key.size() > 0)
            {
                if (key == "listen" && isLocation == false)
                    parseListen(value);
                else if (key == "server_name" && isLocation == false)
                {
                    if (singleServer.serverName.size() == 0)
                        parsServerName(value);
                    else
                    {
                        std::cout << "Error: duplicate server_name directive\n";
                        exit(1);
                    }
                }
                else if ((key == "index" || key == "root") && isLocation == false)
                {
                    if (key == "root")
                    {
                        if (singleServer.root.size() == 0)
                            parsRootIndex(value, key);
                        else
                        {
                            std::cout << "Error: duplicate root directive\n";
                            exit(1);
                        }
                    }
                    if (key == "index")
                    {
                        if (singleServer.index.size() == 0)
                            parsRootIndex(value, key);
                        else
                        {
                            std::cout << "Error: duplicate index directive\n";
                            exit(1);
                        }
                    }
                }
                else if (key == "client_max_body_size" && isLocation == false)
                {
                    if (singleServer.maxBodySize == -1)
                        parsMaxBodySize(value);
                    else
                    {
                        std::cout << "Error: duplicate client_max_body_size directive\n";
                        exit(1);
                    }
                }
                else if (key == "error_page" && isLocation == false)
                    parsError_page(value);
                else if (key == "location")
                {
                    if (checkLocationBrace == 0)
                    {
                        isLocation = true;
                        pos = _serverBlocks[i].find('\n', 0);
                        if (value[0] != '/') value = '/' + value;
                        if (value[value.size()-1] == ';')
                        {
                            std::cout << "Error: directive location has no opening '{'\n";
                            exit(1);
                        }
                        singleServer.servLoc.path = value;
                        checkLocationBrace = 2;
                    }
                    else
                    {
                        std::cout << "Error: invalid location block\n";
                        exit(1);
                    }
                }
                else if (checkLocationBrace == 2)
                {
                    if (key != "{")
                    {
                        std::cout << "Error: invalid location block\n";
                        exit(1);
                    }
                    checkLocationBrace--;
                }
                else if (key == "}")
                {
                    if (isLocation)
                    {
                        isLocation = false;
                        for (int i = 0; i < (int)singleServer.loc.size(); i++)
                        {
                            if (singleServer.loc[i].path == singleServer.servLoc.path)
                            {
                                std::cout << "Error: duplicate location block\n";
                                exit(1);
                            }
                        }
                        singleServer.loc.push_back(singleServer.servLoc);
                        singleServer.servLoc = Location();
                        checkLocationBrace--;
                    }
                }
                else if (checkLocationBrace == 1)
                {
                    parsLocation(key, value);
                }
                else 
                {
                    std::cout << "Error: invalid directive\n";
                    exit(1);
                }
            }
            else
            {
                std::cout << "Error: invalid directive\n";
                exit(1);
            }
        }

  


        // check if all directives of server are filled
        if (singleServer._listen.size() == 0)
        {
            std::cout << "Error: listen directive is missing\n";
            exit(1);
        }
        if (singleServer.root.size() == 0)
        {
            std::cout << "Error: root directive is missing\n";
            exit(1);
        }
        if (singleServer.maxBodySize == 0 || singleServer.maxBodySize == -1)
        {
            singleServer.maxBodySize = 1000000;
        }
        if (isServer == true)
        {
            servers.push_back(singleServer);
            singleServer = Server();
        }
    }




    // print the vector of server blocks
    // std::cout << servers.size() << std::endl;
    // std::cout <<servers[0].loc.size() << std::endl;
    // for(int i = 0; i < (int)servers.size(); i++)
    // {
    //     std::cout << "\n--------********server = "<< i << "***********-----------\n\n";
    //     for (int j = 0; j < (int)servers[i]._listen.size(); j++)
    //     {
    //         std::cout << "listen = " << servers[i]._listen[j].first << ":" << servers[i]._listen[j].second << std::endl;
    //     }
    //     std::cout << "root = " << servers[i].root << std::endl;
    //     std::cout << "index = " << servers[i].index << std::endl;
    //     std::cout << "serverName = " << servers[i].serverName << std::endl;
    //     std::cout << "maxBodySize = " << servers[i].maxBodySize << std::endl;
    //     std::cout << "----------location--------------\n";
    //     for (int j = 0; j < (int)servers[i].loc.size(); j++)
    //     {
    //         std::cout << "path = " << servers[i].loc[j].path << "|" << std::endl;
    //         std::cout << "root = " << servers[i].loc[j].root << "|" << std::endl;
    //         std::cout << "index = " << servers[i].loc[j].index << "|" << std::endl;
    //         std::cout << "autoIndex = " << servers[i].loc[j].autoIndex << "|" << std::endl;
    //         std::cout << "allowed_methods = " << servers[i].loc[j].methods[0] << " " << servers[i].loc[j].methods[1] << " " << servers[i].loc[j].methods[2] << std::endl;
    //         std::cout << "return = " << servers[i].loc[j].redir.first << " " << servers[i].loc[j].redir.second << std::endl;
    //         for (auto el: servers[i].loc[j].errorPage)
    //         {
    //             std::cout << "errorPage = " << el.first << " " << el.second << std::endl;
    //         }
    //         for (auto el: servers[i].loc[j].cgiPath)
    //         {
    //             std::cout << "cgiPath = " << el.first << " " << el.second << std::endl;
    //         }
    //         std::cout << "-------------location-------------------\n";
    //     }

    // }
}

// do a check variable to check if a pass a location block pass the curly bracket
