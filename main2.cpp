#include "server.hpp"
#include "request.hpp"
#include <poll.h>

#define BUFFER_SIZE 32664

void    push_fd(int new_fd_socket, std::vector<pollfd> fd)
{
    struct pollfd fds;
    fds.fd = new_fd_socket;
    fds.events = POLLIN;
    fd.push_back(fds);
    fd.push_back(fds);
}



// int main() {
    
//     std::vector<int> server_fd;
//     std::vector<int> client;
//     std::vector<pollfd> fd;

//     server a = server(5001, fd, server_fd);
//     a.loop_serv(server_fd, client, fd);

//     return (0);
// }

int main() {
    
    std::vector<int> server_fd;
    std::vector<int> client;
    std::vector<pollfd> fd;
    std::vector<server> all_server; 
    
    // all_server.push_back(server(5001, fd, server_fd));
    // all_server.push_back(server(5002, fd, server_fd));
    // all_server.push_back(server(5003, fd, server_fd));
    // all_server.push_back(server(5004, fd, server_fd));
    // all_server.push_back(server(5006, fd, server_fd));
    // all_server.push_back(server(5007, fd, server_fd));
    // all_server.push_back(server(5008, fd, server_fd));
    // all_server.push_back(server(4040, fd, server_fd));
    // all_server.push_back(server(5050, fd, server_fd));
    // all_server.push_back(server(5051, fd, server_fd));
    // all_server.push_back(server(5052, fd, server_fd));
    // all_server.push_back(server(5053, fd, server_fd));



    int port_numbers[] = {5001, 5002, 5003, 5004, 5006, 5007, 5008, 4040, 5050, 5051, 5052, 5053};
    int num_ports = sizeof(port_numbers) / sizeof(port_numbers[0]);

    for (int i = 0; i < num_ports; ++i) {
        all_server.push_back(server(port_numbers[i], fd, server_fd));
    }


    // std::set<int> tab_port;
    // tab_port.insert(9000);
    // tab_port.insert(5001);
    // tab_port.insert(5260);
    char str[BUFFER_SIZE];
    size_t reqSize = 0;
    size_t recvSize = 0;
    std::string tmpr;
    // std::vector<size_t> socketsToRemove;
    
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
                    bzero(str, BUFFER_SIZE);
                    size_t tmpRecvSize = read(fd[i].fd, str, BUFFER_SIZE);
                    recvSize += tmpRecvSize;
                    tmpr.append(str, tmpRecvSize);
                    // puts("|||||||||||||||||||||||||||");
                    // std::cout << tmpr << std::endl;
                    // puts("|||||||||||||||||||||||||||");

                    size_t endOfHeader = tmpr.find("\r\n\r\n");
                    if (endOfHeader != std::string::npos && reqSize == 0) {
                        size_t startOfContentLength = tmpr.find("Content-Length:", 0);
                        if (startOfContentLength != std::string::npos) 
                        {
                            size_t endOfContentLength = tmpr.find("\r\n", startOfContentLength);
                            reqSize = std::stoi(tmpr.substr(startOfContentLength + 15, endOfContentLength - startOfContentLength - 15));
                        }
                    }

                    if (recvSize >= reqSize + endOfHeader + 4) {
                        fd[i].revents = POLLOUT;
                        // tmpRecvSize = 0;
                        // recvSize = 0;
                        // tmpr.clear();
                        // endOfHeader = 0;
                    }
                }
            }
            if (fd[i].revents & POLLOUT) 
            {

                 	request req(tmpr);
                    req.parse();
                
                std::ifstream file("index.html");
                std::cout << str <<"\n";
                std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                std::string buff;
                std::string tmp;
                while (getline(file, tmp))
                {
                    buff += tmp;
                    buff += "\n";
                }
                buff.insert(0, res);
                buff += "\r\n";
                write(fd[i].fd, (char *)(buff.data()) , buff.length());
                close(fd[i].fd);
                fd.erase(fd.begin() + i);
                // puts("|||||||||||||||||||||||||||||||||");
                // client.erase(client.begin() + i); 
      
            }
            if (fd[i].revents & POLLERR)
            {
                std::cout << "Erreur on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
                // Remove the socket from the vectors and adjust the index
                fd.erase(fd.begin() + i);
                for (size_t j = 0; j < client.size(); j++)
                {
                    std::vector<int>::iterator it = std::find(client.begin(), client.end(), fd[i].fd);
                    if (it != client.end())
                    {
                        client.erase(client.begin() + j);
                    }
                }
                puts("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
                
            }
            if (fd[i].revents & POLLHUP)
            {
                std::cout << "Hang up on socket " << fd[i].fd << std::endl;
                close(fd[i].fd);
                // Remove the socket from the vectors and adjust the index
                fd.erase(fd.begin() + i);
                for (size_t j = 0; j < client.size(); j++)
                {
                    std::vector<int>::iterator it = std::find(client.begin(), client.end(), fd[i].fd);
                    if (it != client.end())
                    {
                        puts("supprime fd_client");
                        client.erase(client.begin() + j);
                    }
                }
                
                // client.erase(client.begin() + i);
                puts("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            }
            if (fd[i].revents  & POLLNVAL) 
            {
                    close(fd[i].fd);
                    fd.erase(fd.begin() + i);
                    for (size_t j = 0; j < client.size(); j++)
                {
                    std::vector<int>::iterator it = std::find(client.begin(), client.end(), fd[i].fd);
                    if (it != client.end())
                    {
                        client.erase(client.begin() + j);
                    }
                }
                    puts("++++++++++++++++++++++++++++");
            }
        }
    }



    return 0;
}
