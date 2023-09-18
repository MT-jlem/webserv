#include <poll.h>
#include <climits>
#include <fcntl.h>
#include <iostream>
#include <sys/poll.h>
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
#include <vector>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "request.hpp"
#include "config/read_config.hpp"
#include "config/conf.hpp"

std::string err = "";
std::map<std::string, std::string> statusCodes;
void initializeEncode();

void statusCodesInitialize(){
	statusCodes["200"] = " OK\r\n";
	statusCodes["201"] = " Created\r\n";
	statusCodes["202"] = " Accepted\r\n";
	statusCodes["203"] = " Non-Authoritative Information\r\n";
	statusCodes["204"] = " No Content\r\n";
	statusCodes["205"] = " Reset Content\r\n";
	statusCodes["206"] = " Partial Content\r\n";
	statusCodes["300"] = " Multiple Choices\r\n";
	statusCodes["301"] = " Moved Permanently\r\n";
	statusCodes["302"] = " Found\r\n";
	statusCodes["303"] = " See Other\r\n";
	statusCodes["304"] = " Not Modified\r\n";
	statusCodes["305"] = " Use Proxy\r\n";
	statusCodes["306"] = " Switch Proxy\r\n";
	statusCodes["307"] = " Temporary Redirect\r\n";
	statusCodes["400"] = " Bad Request\r\n";
	statusCodes["401"] = " Unauthorized\r\n";
	statusCodes["402"] = " Payment Required\r\n";
	statusCodes["403"] = " Forbidden\r\n";
	statusCodes["404"] = " Not Found\r\n";
	statusCodes["405"] = " Method Not Allowed\r\n";
	statusCodes["406"] = " Not Acceptable\r\n";
	statusCodes["407"] = " Proxy Authentication Required\r\n";
	statusCodes["408"] = " Request Time-out\r\n";
	statusCodes["409"] = " Conflict\r\n";
	statusCodes["410"] = " Gone\r\n";
	statusCodes["411"] = " Length Required\r\n";
	statusCodes["412"] = " Precondition Failed\r\n";
	statusCodes["413"] = " Request Entity Too Large\r\n";
	statusCodes["414"] = " Request-URI Too Long\r\n";
	statusCodes["415"] = " Unsupported Media Type\r\n";
	statusCodes["416"] = " Requested range not satisfiable\r\n";
	statusCodes["417"] = " Expectation Failed\r\n";
	statusCodes["500"] = " Internal Server Error\r\n";
	statusCodes["501"] = " Not Implemented\r\n";
	statusCodes["505"] = " HTTP Version Not Supported\r\n";
}

//❌❌❌❌❌❌❌ close all the fds

// void handler(int){
// 	std::cout << "i'm out here\n";
// 	close(sock);
// 	exit(1);
// }

int main(int ac, char *av[]){

	Conf config(ac, av[1]);
	std::vector<pollfd> fds;
	config.readingFile();
	config.checkBraces();
	config.fill_Directives_Locations();

	for (int i = 0; i < config.servers.size(); ++i){
		config.servers[i].createServer();
		fds.insert(fds.end(), config.servers[i].fds.begin(), config.servers[i].fds.begin());
	}
	statusCodesInitialize();
	initializeEncode();
	int rv;
	while (1) {
		// rv = poll()
	}

}