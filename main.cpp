#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <poll.h>
#include <climits>
#include <fcntl.h>
#include <iostream>
#include <random>
#include <strings.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/signal.h>
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
#include <utility>
#include <vector>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "request.hpp"
#include "config/read_config.hpp"
#include "config/conf.hpp"
#include "client.hpp"
//host connection chunked in res 



void first_req(Client &ref){
	size_t pos;
	ref.firstTime = false;
	if (std::string::npos != (pos = ref.req.find("Content-Length:"))){
		ref.read -= ref.req.find("\r\n\r\n");
		size_t end = ref.req.find("\n", pos);
		ref.content_length = std::stoi(ref.req.substr(pos + 16, end - (pos + 16)));
	} else if (std::string::npos != (pos = ref.req.find("Transfer-Encoding: chunked"))) {
		ref.chunked = true;
	}

}

std::string err = "";
std::map<std::string, std::string> statusCodes;
void initializeEncode();

int find_server(std::vector<Server> &servers, request &req, int fd){
	size_t start = req.rawReq.find("Host: ");
	if (start == std::string::npos){
		err = "400";
		return 0;
	}
	std::string host = req.rawReq.substr(start + 6, req.rawReq.find("\r\n", start) - (start + 6));
	if (host.find("localhost") != host.npos){
		host.replace(0, 9, "127.0.0.1");
	}
	int k = -1;

	for (size_t i = 0; i <  servers.size(); ++i){
		if (servers[i].serverName != ""){
			if (servers[i].serverName == host)
				return i;
		}
		for (size_t j = 0; j < servers[i]._listens.size(); ++j){
			if (servers[i]._listens[j] == host && k < 0)
				k = i;
		}
	}
	if (k < 0){
		for (size_t i = 0; i <  servers.size(); ++i){
			for (size_t j = 0; j < servers[i].fd.size(); ++j){
				if (servers[i].fd[j] == fd)
					return i;
			}
		}
	}
	return 0;
}


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

int main(int ac, char *av[]){

	try {
		Conf config(ac, av[1]);
		std::vector<pollfd> fds;
		std::vector<int> servers;
		std::map<int , Client> clients;
		std::vector<std::string> created;

		config.readingFile();
		config.checkBraces();
		config.fill_Directives_Locations();
		for (size_t i = 0; i < config.servers.size(); ++i){
			if (config.servers[i].createServer(created)){
				for (int j = i; j >= 0; --j)
					config.servers[j].closeFd();
				std::cout << "error while creating servers\n";
				return -1;
			}
			fds.insert(fds.end(), config.servers[i].fds.begin(), config.servers[i].fds.end());
			servers.insert(servers.end(), config.servers[i].fd.begin(), config.servers[i].fd.end());
		}

		statusCodesInitialize();
		initializeEncode();
		int rv;
		// config.servers[0].loc[0].upload = "/Users/mjlem/Desktop/upload/";
		signal(SIGPIPE, SIG_IGN);
		while (1) {
			rv = poll(fds.data(), fds.size(), -1);
			if (rv < 0){
				std::cout << "poll() failed\n";
				exit(1);
			}
			else if (rv){
				for (size_t i = 0; i < fds.size(); ++i){
					if (fds[i].revents & (POLLHUP |  POLLNVAL | POLLERR ))
					{
						close(fds[i].fd);
						clients.erase(fds[i].fd);
						fds.erase(fds.begin() + i);

					}
					else if (fds[i].revents & POLLIN){
						std::vector<int>::iterator it = std::find(servers.begin(), servers.end(), fds[i].fd);
						if (it != servers.end()){
							int cfd;
							cfd = accept(fds[i].fd, NULL, NULL);
							if (cfd < 0){
								std::cout << "accept() failed\n";
								std::strerror(errno);
								continue;
							}
							fcntl(cfd, F_SETFL, O_NONBLOCK);
							pollfd tmp;
							tmp.fd = cfd;
							tmp.events = POLLIN;
							fds.push_back(tmp);
							clients.insert(std::make_pair(fds[i].fd, Client()));
							// clients[cfd].servIndex = find_server(config.servers, fds[i].fd);
						} 
						else {
							char buff[BUFFER_SIZE];
							bzero(&buff, BUFFER_SIZE);
							Client &client = clients[fds[i].fd];
							long long recv = read(fds[i].fd, buff, BUFFER_SIZE);
							if (recv == -1){
								err = "500";
								std::cout << "read error\n";
								std::strerror(errno);
								continue;
							}
							else if (recv == 0){
								clients.erase(fds[i].fd);
								close(fds[i].fd);
								fds.erase(fds.begin() + i);
								continue;
							}
							client.req.append(buff, recv);
							client.read	+= recv;
							if (client.firstTime)
								first_req(client);
							if ((client.content_length <= client.read) || (client.chunked && client.req.find("\r\n0\r\n") != std::string::npos)){
								fds[i].events = POLLOUT;
							}
							// bzero(buff, recv);
						}
					}
					else if (fds[i].revents & POLLOUT) {
						Client &ref = clients[fds[i].fd];
						if (!ref.processing)
						{
							request req(ref.req);
							if (err == "")
								ref.servIndex = find_server(config.servers, req, ref.fd);
							if (err == "")
								req.parse(config.servers[ref.servIndex]);
							Response res(req, config.servers[ref.servIndex]);
							res.resBuilder(req, config.servers[ref.servIndex]);
							ref.processing = true;
							ref.__response = res.res;
						}
						if (ref.sendLength < (long)ref.__response.length())
						{
							int res = write(fds[i].fd, ref.__response.c_str() + ref.sendLength, ref.__response.length() - ref.sendLength);
							if (res == -1)
							{
								err = "";
								std::cout << "write failed\n";
								close(fds[i].fd);
								clients.erase(fds[i].fd);
								fds.erase(fds.begin() + i);
								break;
							} else if (res == 0){
								err = "";
								std::cout << "client closed the connection\n";
								close(fds[i].fd);
								clients.erase(fds[i].fd);
								fds.erase(fds.begin() + i);
								break;
							}
							ref.sendLength += res;
						}
						else
						{
							err = "";
							clients.erase(fds[i].fd);
							close(fds[i].fd);
							fds.erase(fds.begin() + i);
						}
					}
				}
			}
		}
	} catch (std::exception &r) {
		std::cout << r.what() << "\n";
	}

}