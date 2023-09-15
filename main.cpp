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
#include "client.hpp"
#define BUFFER_SIZE 1024
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
    std::vector<int> client_;
    std::vector<pollfd> fd;
    std::vector<server> all_server;
    std::vector<client> all_client;

    char str[BUFFER_SIZE];
    std::string tmp;


    // int port_numbers[] = {5001, 5002, 5003, 5004, 5006, 5007, 5008, 4040, 5050, 5051, 5052, 5053};
    int port_numbers[] = {5001};
    // int num_ports = sizeof(port_numbers) / sizeof(port_numbers[0]);

    for (int i = 0; i < 1; ++i) 
    {
        // server serv;
		// initializeServ(serv);
        all_server.push_back(server(port_numbers[i], fd, server_fd));
    }
    // std::cout << "server_size = " << server_fd.size() << std::endl;
    statusCodesInitialize();
    while (1) 
    {
        std::cout<<"dfdsfdsf"<<std::endl;
        int rev = poll(fd.data(), fd.size(), -1);

        if (rev == -1) 
        {
            std::cout << "problem in time poll" << std::endl;
            return 1;
        }
        std::cout<<"dfdsfdsf"<<std::endl;
        for (size_t i = 0; i < fd.size(); i++) 
        {
			server serv;
			initializeServ(serv);
            if (fd[i].revents & POLLIN) 
            {
                std::vector<int>::iterator it = std::find(server_fd.begin(), server_fd.end(), fd[i].fd);
                if (it != server_fd.end())
                {
                    int new_fd_socket;
                    new_fd_socket = accept(fd[i].fd, (struct sockaddr *)&all_server[std::distance(server_fd.begin(),it)].client_address , (socklen_t *)&all_server[std::distance(server_fd.begin(),it)].adresslen);
                    if (new_fd_socket < 0) 
                    {
                        std::cout << "erreur acceptation" << std::endl;
                        
                        break;
                    }
                    fcntl(new_fd_socket, F_SETFL, O_NONBLOCK);
                    struct pollfd fds;
                    fds.fd = new_fd_socket;
                    fds.events = POLLIN;
                    std::cout << "accepte dazt\n";
                    fd.push_back(fds);
                    client_.push_back(new_fd_socket);
                    all_client.push_back(client(new_fd_socket));
                } 
                else 
                {


                    long long start;
                    tmp = "";
                    bzero(str, BUFFER_SIZE);
                    std::cout << "before" << std::endl;
                    start = read(fd[i].fd, str, BUFFER_SIZE);
                    std::cout << "start === " << start << std::endl;
                    if (start == -1)
                    {
                        for (size_t j = 0; j < all_client.size(); j++)
                        {
                            if (fd[i].fd == all_client[j].fd_socket)
                            {
                                close(fd[i].fd);
                                fd.erase(fd.begin() + i);
                                all_client.erase(all_client.begin() + j);
                                std::cout << "delete client\n";
                                break;
                            }

                        }
                    }
                    // //find content lenght  one more
                    for (size_t j = 0; j < all_client.size(); j++)
                    {
                        if (fd[i].fd == all_client[j].fd_socket)
                        {
                            if (start == 0 && !all_client[j].first_requset)
                            {
                                close(fd[i].fd);
                                all_client.erase(all_client.begin() + j);
                                fd.erase(fd.begin() + i);
                                break;
                            }
                            // all_client[j].req.append(str,BUFFER_SIZE);
                            // all_client[j].a_lire+=1024;
                            all_client[j].take_servername();
                            all_client[j].find_content(str,start);
                            all_client[j].debut_a_lire = start;
                                // all_client[j].first_requset = false;
                                // std::cout << "may content = " << all_client[j].valeur_content_len << std::endl;
                            if((all_client[j].req.length() && !all_client[j].ischenked && all_client[j].req.find("\r\n\r\n") != std::string::npos  &&  all_client[j].valeur_content_len <= all_client[j].debut_a_lire) 
                                || ( all_client[j].ischenked && all_client[j].req.find("\r\n0\r\n\r\n") != std::string::npos) )
                            {
                                std::cout << "hey\n";
                                //   std::ofstream outputFile("show.txt");
                                // if (!outputFile.is_open()) {
                                //     std::cerr << "Failed to open the file for writing." << std::endl;
                                //     return 1;
                                // }
                                // outputFile << all_client[j].req;
                                // outputFile.close();
                                // std::cout << ""
                                // tmp.append(all_client[j].req, BUFFER_SIZE);
                                // std::cout << "tmp ||||||" << tmp << "|||||||" << std::endl;
                                // all_client[j].req="";
                                fd[i].events = POLLOUT;
		                    }
                        }

                    }
        
                    bzero(str, BUFFER_SIZE);
                } 
            }
            
            else if (fd[i].revents & POLLOUT) 
            {
                //socket pret a ecriture
                size_t j = 0;
                while (j < all_client.size())
                {
                    if (all_client[j].fd_socket == fd[i].fd)
                    {
                        break;
                    }
                    j++;
                }
              
                if (all_client[j].traiter != true)
                {
                    err = "";
                    request req(all_client[j].req);
                    req.parse(serv);
                    Response resp(req, serv);
                    std::string buff;
                    buff = resp.res;
                    std::cout << "poolout\n";
                    write(fd[i].fd, (char *)(buff.data()) , buff.length());
                    all_client[j].traiter = true;
                    // close(fd[i].fd);
                    puts("la");
                    // fd.erase(fd.begin() + i);
                    // all_client.erase(all_client.begin() + j);
                    // all_client.erase(fd.begin() + j);
                }
                else if (all_client[j].traiter == true)
                {
                    close(fd[i].fd);
                    fd.erase(fd.begin() + i);
                    all_client.erase(all_client.begin() + j);
                    puts("il est traite donc je supprime le client");
                }
                    
            
                // err = "";
                // request req(all_client[j].);

                // req.pars                 


      
            }
            else if (fd[i].revents & POLLERR)
            {
      
                std::cout << "Erreur on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
          
                // Remove the socket from the vectors and adjust the index
                fd.erase(fd.begin() + i);
                
            }
            else if (fd[i].revents & POLLHUP)
            {
                std::cout << "Hang up on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
 
                fd.erase(fd.begin() + i);
                
            }
            else if (fd[i].revents  & POLLNVAL) 
            {
                  std::cout << "Erreur on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);

                fd.erase(fd.begin() + i);

            }
        }
    }

    return 0;
}