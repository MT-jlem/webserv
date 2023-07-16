#include <iostream>
#include <sstream>
#include <string>

int main (){
	std::istringstream rawReq("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!\r\n\r\nweeeee");
	std::string tmp;

		while(std::getline(rawReq, tmp, '\n')){
		std::cout << tmp << "\n";
	}
}