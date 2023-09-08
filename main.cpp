#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "request.hpp"
#include <poll.h>
#define BUFFER_SIZE 32664
// ❌❌❌❌❌ use multimap in req headers
std::string err = "";
std::map<std::string, std::string> statusCodes;
/*to-do:
	- parse conf file
		create http class
	- create the servers
		create server class
	- get the req (parse, ...)
		create a req class
	- proccess the req and send the response (read files, cgi,....)
		create a response string
	- cookies, session ...
*/

	//getaddrinfo
	//socket
	//setsockopt
	//bind
	// fcntl
	//listen
	//accept
	//poll
	//send || //recv
	//PS: fcntl(sock, F_SETFL, O_NONBLOCK);//should be used with poll() for non-blocking i/o operations
/*
			"300"  ; Section 10.3.1: Multiple Choices
          | "301"  ; Section 10.3.2: Moved Permanently
          | "302"  ; Section 10.3.3: Found
          | "303"  ; Section 10.3.4: See Other
          | "304"  ; Section 10.3.5: Not Modified
          | "305"  ; Section 10.3.6: Use Proxy
          | "307"  ; Section 10.3.8: Temporary Redirect
*/
void statusCodesInitialize(){
	statusCodes["200"] = " OK\r\n";
	statusCodes["204"] = " No Content\r\n";
	statusCodes["201"] = " Created\r\n";
	statusCodes["300"] = " Multiple Choices\r\n";
	statusCodes["301"] = " Moved Permanently\r\n";
	statusCodes["302"] = " Found\r\n";
	statusCodes["303"] = " See Other\r\n";
	statusCodes["304"] = " Not Modified\r\n";
	statusCodes["305"] = " Use Proxy\r\n";
	statusCodes["306"] = " Switch Proxy\r\n";
	statusCodes["307"] = " Temporary Redirect\r\n";
	statusCodes["400"] = " Bad Request\r\n";
	statusCodes["403"] = " Forbidden\r\n";
	statusCodes["404"] = " Not Found\r\n";
	statusCodes["405"] = " Method Not Allowed\r\n";
	statusCodes["409"] = " Conflict\r\n";
	statusCodes["413"] = " Request Entity Too Large\r\n";
	statusCodes["414"] = " Request-URI Too Long\r\n";
	statusCodes["500"] = " Internal server Error\r\n";
	statusCodes["501"] = " Not Implemented\r\n";
	statusCodes["505"] = " HTTP Version Not Supported\r\n";
}

void initializeServ(server &serv){
	//if root doesn't have "/" in the end add "/"
	serv.root = "/Users/tbouzalm/Desktop/webserv/";
	// serv.index = "index.html";
	serv.maxBodySize = 10000000;
	serv.loc.resize(2);
	serv.loc[0].path = "/";
	serv.loc[0].autoIndex = true;
	serv.loc[0].methods[GET] = 1;
	serv.loc[0].methods[POST] = 1;
	serv.loc[0].methods[DELETE] = 1;
	// serv.loc[0].redir.first = "301";
	// serv.loc[0].redir.second = "https://youtu.be/3f72kbqN6hg?t=99";
	serv.loc[1].path = "/loc";
	serv.loc[1].autoIndex = true;
	serv.loc[1].methods[GET] = 1;
	serv.loc[1].methods[POST] = 1;
	serv.loc[1].methods[DELETE] = 1;
}

int main() {
    
    std::vector<int> server_fd;
    std::vector<int> client;
    std::vector<pollfd> fd;
    std::vector<server> all_server; 

    char str[BUFFER_SIZE];
    size_t reqSize;
    size_t start;
    size_t recvSize = 0;
    size_t end;
    std::string tmp;


    // int port_numbers[] = {5001, 5002, 5003, 5004, 5006, 5007, 5008, 4040, 5050, 5051, 5052, 5053};
    int port_numbers[] = {5001};
    int num_ports = sizeof(port_numbers) / sizeof(port_numbers[0]);

    for (int i = 0; i < num_ports; ++i) 
    {
        // server serv;
		// initializeServ(serv);
        all_server.push_back(server(port_numbers[i], fd, server_fd));
    }

    statusCodesInitialize();
    while (1) 
    {
        // std::cout<<"dfdsfdsf"<<std::endl;
        int rev = poll(fd.data(), fd.size(), -1);

        if (rev == -1) 
        {
            std::cout << "problem in time poll" << std::endl;
            return 1;
        }

        for (size_t i = 0; i < fd.size(); i++) 
        {
            if (fd[i].revents & POLLIN) 
            {
                std::vector<int>::iterator it = std::find(server_fd.begin(), server_fd.end(), fd[i].fd);
                if (it != server_fd.end()) 
                {
                    int new_fd_socket;
                    if ((new_fd_socket = accept(*it, (struct sockaddr *)&all_server[std::distance(server_fd.begin(),it)].client_address , (socklen_t *)&all_server[std::distance(server_fd.begin(),it)].adresslen)) < 0) 
                    {
                        std::cout << "erreur acceptation" << std::endl;
                        exit(1);
                    }
                    // std::cout << "adresse_ip_client = "  << serv_addr.sin_addr.s_addr << std::endl;
                    fcntl(new_fd_socket, F_SETFL, O_NONBLOCK);
                    // push_fd(new_fd_socket, fd);
                    struct pollfd fds;
                    fds.fd = new_fd_socket;
                    fds.events = POLLIN;
                    fd.push_back(fds);
                    client.push_back(new_fd_socket);
                    // std::cout << "accept client and push fd_socket_client in tab fd and in struct fds " << new_fd_socket << std::endl;
                }
                else 
                {

                    tmp = "";
                    bzero(str, BUFFER_SIZE);
                    recvSize += read(fd[i].fd, str, BUFFER_SIZE);
                    std::cout << "=================RECV===============\n";
                    tmp.append(str, recvSize);
                    recvSize -= tmp.find("\r\n\r\n") + 4;
                    bzero(str, BUFFER_SIZE);


                    //check Content-Length and recv all the req
                    start = tmp.find("Content-Length:");
                    if (start != tmp.npos)
                    {
                        end = tmp.find("\n", start);
                        reqSize = std::stoi(tmp.substr(start + 16, end - 1));
                        while(1)
                        {
                            if (recvSize >= reqSize)
                            {
                                fd[i].revents = POLLOUT;
                                break;
                            }
                            size_t tmpRecvSize = 0;
                            tmpRecvSize =  read(fd[i].fd, str, BUFFER_SIZE);
                            recvSize += tmpRecvSize;
                            tmp.append(str, tmpRecvSize);
                            bzero(str, BUFFER_SIZE);
                        }
                    }
                    else
                        fd[i].revents = POLLOUT;
                }
            }
			server serv;
			initializeServ(serv);
            if (fd[i].revents & POLLOUT) 
            {
                err = "";
                // std::cout << tmp << "\n";
                request req(tmp);

                req.parse(serv);
                Response resp(req, serv);
				std::string buff;
				buff = resp.res;
                std::cout << buff;

                write(fd[i].fd, (char *)(buff.data()) , buff.length());
                // send(fd[i].fd,(char *)(buff.data()), buff.size(),0);
                puts("|||||||||||||||||");
                close(fd[i].fd);
                // fd.erase(fd.begin() + i);
      
            }
            if (fd[i].revents & POLLERR)
            {
                std::cout << "Erreur on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
                // Remove the socket from the vectors and adjust the index
                // fd.erase(fd.begin() + i);
                // for (size_t j = 0; j < client.size(); j++)
                // {
                //     std::vector<int>::iterator it = std::find(client.begin(), client.end(), fd[i].fd);
                //     if (it != client.end())
                //     {
                //         client.erase(client.begin() + j);
                //     }
                // }
                fd.erase(fd.begin() + i);
                
            }
            if (fd[i].revents & POLLHUP)
            {
                std::cout << "Hang up on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
                // Remove the socket from the vectors and adjust the index
                // fd.erase(fd.begin() + i);
                // for (size_t j = 0; j < client.size(); j++)
                // {
                //     std::vector<int>::iterator it = std::find(client.begin(), client.end(), fd[i].fd);
                //     if (it != client.end())
                //     {
                //         puts("supprime fd_client");
                //         client.erase(client.begin() + j);
                //     }
                // }
                fd.erase(fd.begin() + i);
                // client.erase(client.begin() + i);
            }
            if (fd[i].revents  & POLLNVAL) 
            {
                close(fd[i].fd);
                fd.erase(fd.begin() + i);
            }
        }
    }

    return 0;
}