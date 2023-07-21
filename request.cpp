#include "request.hpp"
#include <vector>
#include <iostream>
#include <string>

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
void	request::parse(const std::string &body){

	size_t i = 0, start = 0;
	std::string	tmp;
	std::cout << body;

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
	// if (checkPath()){

	// }
	// while (i < reqSize){
	// 	++i;	
	// }

	// headers = getHeader();
	// body = getBody();

	// if (checkHeaders()){

	// }
}
