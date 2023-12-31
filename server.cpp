#include "server.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/errno.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <system_error>
#include <unistd.h>

Location::Location()
{
    path = "";
    root = "";
    index = "";
    autoIndex = false;
    trackAutoIndex = false;
    methods[0] = 0;
    methods[1] = 0;
    methods[2] = 0;
    redir = std::make_pair("", "");
    upload = "";
}




Location::~Location(){}

Server::Server(){
    _listen = std::vector<std::pair<std::string, std::string> >();
    root = "";
    index = "";
    serverName = "";
    maxBodySize = -1;
    errorPage = std::map<std::string, std::string>();
    memset(&hints, 0, sizeof(hints));
}

Server::~Server(){}

int    Server::createServer(std::vector<std::string> &created){
    hints.ai_socktype = SOCK_STREAM;
    for (size_t i = 0; i < _listen.size(); ++i){
        std::string host = _listen[i].first + ":";
        host += _listen[i].second;
        if (std::find(created.begin(), created.end(), host) == created.end()){
            int rv = getaddrinfo(_listen[i].first.c_str(), _listen[i].second.c_str(), &hints, &servInfo);
            std::cout << "go to http://" << _listen[i].first.c_str() << ":" << _listen[i].second.c_str() << '\n';
            if (rv){
                std::cerr << gai_strerror(rv) << '\n';
                return -1;
            }
            int sock;
            sock = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
            if (sock < 0){
                std::cerr << "socket() failed\n";
                std::strerror(errno);
                return -1;
            }
            int ov = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ov, sizeof(int))){
                std::strerror(errno);
                std::cerr << "setsockopt() failed\n";
                close(sock);
                return -1;
            }
            if (bind(sock, servInfo->ai_addr, servInfo->ai_addrlen) == -1){
                std::cerr << "bind() failed\n";
                std::strerror(errno);
                close(sock);
                return -1;
            }
            if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1){
                std::strerror(errno);
                std::cerr << "fcntl() failed\n";
                close(sock);
                return -1;
            }
            if (listen(sock, SOMAXCONN)){
                std::strerror(errno);
                std::cerr << "listen() failed\n";
                close(sock);
                return -1;
            }

            fd.push_back(sock);
            pollfd tmp;
            tmp.fd = sock;
            tmp.events = POLLIN;
            fds.push_back(tmp);
            freeaddrinfo(servInfo);
            created.push_back(host);
        }
    }
    return 0;
}

void Server::closeFd(){
    for (size_t i = 0; i < fd.size(); ++i){
        close(fd[i]);
    }
}