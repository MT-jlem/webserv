/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbouzalm <tbouzalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 01:13:32 by tbouzalm          #+#    #+#             */
/*   Updated: 2023/08/29 15:44:23 by tbouzalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
// #include <sys/fcntl.h>


int server::create_socket()
{
    int fd_socket;

    fd_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_socket <= -1)
    {
        std::cout << "erreur creation du socket!!!" << std::endl;
        exit(1);
    }
    return (fd_socket);
}

void server::binding_socket(int fd_socket)
{
    int reuse = 1;
    if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        std::cout << "Failed to set socket option" << std::endl;
        return ;
    }
    if (bind(fd_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= -1)
    {
        std::cout << "serveur: erreur d'affecter l'adresse ip ou port" << std::endl;
        exit(1);
    }
}

void server::listening_socket(int fd_socket)
{
    if (listen(fd_socket, 10000) == -1)
    {
        std::cout << "erreur au ecoute!!!" << std::endl;
        exit(1);
    }
}

// int server::accepte_connect(int fd_socket)
// {
//     int new_fd_socket;

//     new_fd_socket = accept(fd_socket, (struct sockaddr *)&serv_addr, (socklen_t *)&adresslen);
//     if (new_fd_socket <= -1)
//     {
//         std::cout << "erreur d'acceptation!!!!!" << std::endl;
//         exit(0);
//     }
//     return (new_fd_socket);
// }

const int &server::get_fd_srv()
{
    return (this->fd_socket);
}

void    server::get_info_server()
{
    std::ifstream file("default.toml");
    
    if (file.is_open() != 1)
    {
        std::cout << "erreur open config file" << std::endl;
        exit(1); 
    }
    
    // std::vector<char>::iterator it = file;
}

server::server(int port, std::vector<pollfd> &fd,std::vector<int> &server_fd)
{
    adresslen = sizeof(serv_addr);
    // const int e = 1;
    fd_socket = create_socket();
    fcntl(fd_socket, F_SETFL, O_NONBLOCK);
    
    std::cout << "la socket est bien cree" << std::endl;
    // if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &e, sizeof(int)) < 0)
    // {
    //     std::cerr << "setsockopt: " << strerror(errno) << std::endl;
    //     exit(1);
    // }
    // std::vector<char> tab_all;
    // get_info_server();
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    binding_socket(fd_socket);
    listening_socket(fd_socket);
    // std::cout << "adresse_ip_server = "  << serv_addr.sin_addr.s_addr << std::endl;
    struct pollfd fds;
    fds.fd =fd_socket ;
    fds.events = POLLIN;
    fd.push_back(fds);
    server_fd.push_back(fd_socket);
}


