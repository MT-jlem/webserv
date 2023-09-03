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
#include "config/read_config.hpp"
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
	statusCodes["500"] = " Internal Server Error\r\n";
	statusCodes["501"] = " Not Implemented\r\n";
	statusCodes["505"] = " HTTP Version Not Supported\r\n";
}

void initializeServ(Server &serv){
	//if root doesn't have "/" in the end add "/"
	serv.root = "/Users/mjlem/Desktop/webserv/";
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

size_t toHex(const std::string &hex){
	std::stringstream tmp;
	size_t res;
	tmp << std::hex << hex;
	tmp >> res;
	return res;
}
int sock;

void handler(int){
	std::cout << "i'm out here\n";
	close(sock);
	exit(1);
}

int main(int ac, char *av[]) // main for config
{
	ReadConfig readConfig(ac, av[1]);
}


// int main(){
// 	err = "";
// 	statusCodesInitialize();
// 	struct addrinfo *res, hints;

// 	signal(SIGINT, handler);
// 	std::memset(&hints, 0, sizeof(hints));
// 	hints.ai_socktype = SOCK_STREAM;
// 	if (getaddrinfo("127.0.0.1", "8080", &hints, &res) != 0){
// 		std::cerr << "getaddrinfo failed\n";
// 	}

// 	int yes = 1;
// 	for (; res; res = res->ai_next){
// 		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
// 		if (sock < 0){
// 			perror("socket");
// 			continue;
// 		}
// 		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){
// 			perror("setsockopt");
// 			continue;
// 		}
// 		if (bind(sock, res->ai_addr, res->ai_addrlen) != 0){
// 			close(sock);
// 			perror("bind");
// 			continue;
// 		}
// 		listen(sock, SOMAXCONN);
// 		break;
// 	}
// 	if (!res)
// 		return 1;
// 	struct sockaddr_storage s;
// 	socklen_t len;
// 	int host;
// 	while (1)
// 	{
// 		std::cout << "waiting for new connection\n";
// 		host = accept(sock, (sockaddr *)&s, &len);
// 		if (host < 0){
// 			close(sock);
// 			perror("accept");
// 			return 1;
// 		}
// 		char str[BUFFER_SIZE];
// 		size_t reqSize;
// 		size_t start;
// 		size_t recvSize = 0;
// 		size_t end;
// 		std::string tmp;
// 		bzero(str, BUFFER_SIZE);
// 		recvSize += recv(host, str, BUFFER_SIZE , 0);
// 		std::cout << "=================RECV===============\n";
// 		tmp.append(str, recvSize);
// 		recvSize -= tmp.find("\r\n\r\n") + 4;
// 		bzero(str, BUFFER_SIZE);
// 		// if (tmp.find("Transfer-Encoding: chunked") != tmp.npos){
// 		// 	start = tmp.find("\r\n\r\n") + 4;
// 		// 	reqSize = toHex(tmp.substr(start, tmp.find("\n", start) - start));

// 		// }

// 		//check Content-Length and recv all the req
// 		start = tmp.find("Content-Length:");
// 		if (start != tmp.npos){
// 			end = tmp.find("\n", start);
// 			reqSize = std::stoi(tmp.substr(start + 16, end - 1));
// 			while(1){
// 				if (recvSize >= reqSize)
// 					break;
// 				size_t tmpRecvSize = 0;
// 				tmpRecvSize =  recv(host, str, BUFFER_SIZE , 0);
// 				// std::cout << tmpRecvSize << "=================RECV===============\n";
// 				recvSize += tmpRecvSize;
// 				tmp.append(str, tmpRecvSize);
// 			bzero(str, BUFFER_SIZE);

// 			}
// 		}

// 		Server serv;
// 		initializeServ(serv);


// 		{
// 			err = "";
// 			// std::cout << tmp << '\n';
// 			request req(tmp);
// 			req.parse(serv);
// 			Response resp(req, serv);
// 		std::string buff;
// 		buff = resp.res;
// 		send(host,(char *)(buff.data()), buff.size(),0);
// 		close(host);
// 		}
// 		// break;
// 	}
	
// 	freeaddrinfo(res);
// 	close(host);
// 	close(sock);
	
// }