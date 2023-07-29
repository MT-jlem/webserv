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
#include "request.hpp"
#define BUFFER_SIZE 32664

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
int sock;

void handler(int){
	std::cout << "i'm out here\n";
	close(sock);
	exit(1);
}
// int main(int ac, char *av[]){ //takes config file
int main(){

	struct addrinfo *res, hints;

	signal(SIGINT, handler);
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo("127.0.0.1", "8080", &hints, &res) != 0){
		std::cerr << "getaddrinfo failed\n";
	}

	int yes = 1;
	for (; res; res = res->ai_next){
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock < 0){
			perror("socket");
			continue;
		}
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){
			perror("setsockopt");
			continue;
		}
		if (bind(sock, res->ai_addr, res->ai_addrlen) != 0){
			close(sock);
			perror("bind");
			continue;
		}
		listen(sock, SOMAXCONN);
		break;
	}
	if (!res)
		return 1;
	struct sockaddr_storage s;
	socklen_t len;
	int host;
	while (1)
	{
		std::cout << "waiting for new connection\n";
		host = accept(sock, (sockaddr *)&s, &len);
		if (host < 0){
			close(sock);
			perror("accept");
			return 1;
		}
		char str[BUFFER_SIZE];
		size_t reqSize;
		size_t start;
		size_t recvSize = 0;
		size_t end;
		std::string tmpr;
		bzero(str, BUFFER_SIZE);
		recvSize += recv(host, str, BUFFER_SIZE , 0);
		// sleep(1);
		tmpr.append(str, recvSize);
		recvSize -= tmpr.find("\r\n\r\n") + 4;
		bzero(str, BUFFER_SIZE);
		start = tmpr.find("Content-Length:");
		if (start != tmpr.npos){
			end = tmpr.find("\n", start);
			reqSize = std::stoi(tmpr.substr(start + 16, end - 1));
			while(1){
				if (recvSize >= reqSize)
					break;
				size_t tmpRecvSize = 0;
				tmpRecvSize =  recv(host, str, BUFFER_SIZE , 0);
				recvSize += tmpRecvSize;
				tmpr.append(str, tmpRecvSize);
			bzero(str, BUFFER_SIZE);

			}
		}
		//check Content-Length and recv all the req
		{
			request req(tmpr);
			req.parse();
		
		std::ifstream file("index.html");
		// std::cout << str <<"\n";
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
		send(host,(char *)(buff.data()), buff.size(),0);
		close(host);
		}
		// break;
	}
	
	freeaddrinfo(res);
	close(host);
	close(sock);
}