#include "server.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <system_error>

Location::Location()
{
    path = "";
    root = "";
    index = "";
    autoIndex = false;
    methods[0] = 0;
    methods[1] = 0;
    methods[2] = 0;
    redir = std::make_pair("", "");
    upload = "";
}


Location::~Location(){}

Server::Server(){
    memset(&hints, 0, sizeof(hints));
}

Server::~Server(){}

void    Server::createServer(){
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    for (int i = 0; i < _listen.size(); ++i){

        int rv = getaddrinfo(_listen[i].first.c_str(), _listen[i].second.c_str(), &hints, &servInfo);
        if (rv){
            std::cerr << gai_strerror(rv) << '\n';
            exit(1); // close prev fd
        }
        int sock;
        sock = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
        if (sock < 0){
           std::cerr << "socket() failed\n";
           exit(1); // close prev fd
        }
        if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1){
            std::cerr << "fcntl() failed\n";
            exit(1);
        }
        int ov = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &ov, sizeof(int))){
            std::cerr << "setsockopt() failed\n";
            exit(1); // close prev fd
        }
        if (bind(sock, servInfo->ai_addr, servInfo->ai_addrlen)){
            std::cerr << "bind() failed\n";
            exit(1);
        }
        if (listen(sock, SOMAXCONN)){
            std::cerr << "listen() failed\n";
            exit(1);
        }
        fd.push_back(sock);
        
    }
}