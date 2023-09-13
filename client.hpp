/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbouzalm <tbouzalm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 17:44:55 by tbouzalm          #+#    #+#             */
/*   Updated: 2023/07/26 17:47:30 by tbouzalm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <sys/fcntl.h>

class client
{
public:
    int fd_socket;
    std::string req;
    std::string resp;
    int a_lire ;
    int valeur_content_len;
    bool ischenked;
    bool first_requset;
    bool not_cont_chunked;
    std::string servername;
    bool traiter;
public:
    void find_content();
    void take_servername();
    client();
    ~client();    
    client(int fd);

};

void    client::take_servername()
{
    std::string name;
    // int index_port;
    int index_name = req.find("Host") + 6;
    int index_port = req.find("\r\n", index_name);

    // std::cout << "index_name = " << index_name << std::endl;
    // std::cout << "index_port = " << index_port << std::endl;
    if (index_name != -1 && index_port != -1)
    {
        this->servername = req.substr(index_name , index_port - index_name);
        // std::cout << "SERVER_NAME = " << this->servername << std::endl;
    }
}

client::client(int fd)
{
    this->fd_socket = fd;
    first_requset = true;
    req = "";
    valeur_content_len = -1;
    not_cont_chunked = false;
    ischenked = false;
    traiter = false;
}

client::~client()
{

}

void    client::find_content()
{
    size_t start = 0;
    size_t chunked = 0;
    std::cout << req << std::endl;
    start = req.find("Content-Length:");
    chunked = req.find("Transfer-Encoding:");
    if(chunked != std::string::npos)
    {
        ischenked = true;
        first_requset = false;
    }
    else if (start != std::string::npos)
    {

        valeur_content_len = std::stoi(req.substr(start + 16));
        if (valeur_content_len == -1)
        {
            not_cont_chunked = true;
        }
        a_lire = (req.find("\r\n\r\n") + 4) *-1;
        first_requset = false;
    }

}

#endif