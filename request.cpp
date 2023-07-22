#include "request.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

request::request(const std::string &req){
	rawReq = req;
}

request::~request(){}

std::string 						request::getMethod(){
	return method;
}
std::string 						request::getPath(){
	return path;
}
std::string 						request::getVersion(){
	return version;
}
// std::map<std::string, std::string>	request::getHeader(){

// }
// std::string							request::getBody(){

// }
bool								request::checkVerion(){
	return version != "HTTP/1.1";
}
// bool								request::checkPath(){

// }
bool 								request::checkMethod(){
	if (method != "GET" && method != "POST" && method != "DELETE")
		return true;
	return false;
}
// bool								request::checkHeaders(){

// }

size_t	request::parseHeaders(size_t start){

	size_t end, ret;
	std::string tmp;
	end = rawReq.find("\r\n\r\n", start);
	ret = end;
	tmp = rawReq.substr(start, end);
	std::stringstream tab(tmp);
	std::string str;
	while (getline(tab, tmp))
	{
		end = tmp.find(":", 0);
		if (end == tmp.npos)
			break;
		headers[tmp.substr(0, end)] = tmp.substr(end + 2, tmp.size() - (end + 2));
	}

	for ( std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "line >> " << it->first << ":" << it->second << "\n";
	
	return ret;
}

void	request::parse(){

	size_t i = 0, start = 0;
	std::string	tmp;

	i = rawReq.find(" ", 0);//error handling for find and substr
	if (i == rawReq.npos){
		std::cerr << "return an error res\n";
		exit(1);
	}
	method = rawReq.substr(0, i);
	std::cout << method << "-METHOD\n";

	start = rawReq.find(" ", i + 1);
	if (start == rawReq.npos){
		std::cerr << "return an error res\n";
		exit(1);
	}
	path = rawReq.substr(i + 1, start - i - 1);
	std::cout << path << "-PATH\n";

	i = rawReq.find("\r", start + 1);
	if (i == rawReq.npos){
		std::cerr << "return an error res\n";
		exit(1);
	}
	version = rawReq.substr(start + 1, i - start - 1);
	std::cout << version << "-VERSION\n";
	//check for error
	if (checkVerion()){
		std::cerr << "return an error res\n";
		exit(1);
	}
	if (checkMethod()){
		std::cerr << "return an error res\n";
		exit(1);
	}
	start = parseHeaders(i + 2);
	// if (checkPath()){
	// }
	// if (checkHeaders()){

	// }
}

