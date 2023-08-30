/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbouzalm <tbouzalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 00:52:53 by tbouzalm          #+#    #+#             */
/*   Updated: 2023/08/29 16:00:51 by tbouzalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "request.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <set>
#include <sys/fcntl.h>


#include <sys/types.h>

#include <netdb.h>
#include <cstring>
#include <fstream>
#include <signal.h>
#include <string>

#define BUFFER_SIZE 32664

class server
{
    public:
        int fd_socket;
        socklen_t adresslen;
        struct sockaddr_in client_address;
        long count_read;
        struct sockaddr_in serv_addr;
        //  std::vector<pollfd> fd;
    public:
        server(int port,std::vector<pollfd> &fd,std::vector<int> &server_fd);
        const int   &get_fd_srv();
        int     create_socket();
        void    binding_socket(int fd_socket);
        void    listening_socket(int fd_socket);
        int     accepte_connect(int fd_socket);
        void    get_info_server();

};

#endif