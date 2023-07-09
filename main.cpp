#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>

// int main(int ac, char *av[]){ //takes config file
int main(){

	// struct addrinfo *res, hints;

	// std::memset(&hints, 0, sizeof(hints));
	// hints.ai_socktype = SOCK_STREAM;
	// if (getaddrinfo("localhost", "8080", &hints, &res) != 0){
	// 	std::cerr << "getaddrinfo failed\n";
	// }

	// int sock;
	// for (; res; res = res->ai_next){
	// 	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	// 	if (sock < 0){
	// 		std::cerr << "socket failed retry\n";
	// 		continue;
	// 	}
	// 	if (bind(sock, res->ai_addr, res->ai_addrlen) != 0){
	// 		std::cerr << "bind failed retry\n";
	// 		close(sock);
	// 		continue;
	// 	}
	// 	listen(sock, 2);
	// 	break;
	// }
	// struct sockaddr_storage s;
	// socklen_t len;
	// int host;
	// host = accept(sock, (sockaddr *)&s, &len);
	// if (host < 0){
	// 	std::cout << "accept error\n";
	// 	return 1;
	// }
	// char str[2048];
	// recv(host, str, 2048, 0);
	// //reqParser()
	// std::cout << str <<"\n";
	// char *buff = "200 OK\nDate: Tue, 15 Nov 1994 08:12:31 GMT\nServer: CERN/3.0 libwww/2.17\nContent-Type: text/html\n<html>\nA page with an image\n <img SRC="">\n</html>\n";
	// send(host, buff, 2000,0);
	// //socket
	// //setsockopt
	// //bind
	// //listen
	// //accept
	// //poll // fcntl
	// //send || //recv

	// freeaddrinfo(res);
	// close(host);
	// close(sock);

	std::cout << "Aaa\rBv";
}

