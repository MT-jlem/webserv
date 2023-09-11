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
    bool not_cont_chenke;

public:
    void find_content();
    client();
    ~client();    
    client(int fd);

};


client::client(int fd)
{
    this->fd_socket = fd;
    first_requset = true;
    req = "";
    valeur_content_len = -1;
    not_cont_chenke = false;
     ischenked = false;
}

client::~client()
{

}

void    client::find_content()
{
    size_t start = 0;
    size_t ch = 0;
    std::cout << req << std::endl;
    start = req.find("Content-Length:");
    ch = req.find("Transfer-Encoding:");
    if(ch != std::string::npos)
    {
        ischenked = true;
        first_requset = false;
    }
    else if (start != std::string::npos)
    {

        valeur_content_len = std::stoi(req.substr(start + 16));
    if (valeur_content_len == -1)
    {
        not_cont_chenke = true;
    }
    a_lire = (req.find("\r\n\r\n") + 4) *-1;
    first_requset = false;
    }
   


}

#endif