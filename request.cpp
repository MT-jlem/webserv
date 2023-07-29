#include "request.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

// remove boundary + headers from the body 
// handle chunked req && convert size from hex to int and recv the body && remove the boundary
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
bool								request::checkHeaders(){
	if (method == "POST"){
		if (headers.find("Content-Length") == headers.end())// || headers.find("Content-Type") == headers.end()
			return true;
	}
	return false;
}

void	request::parseBody(){
	std::string boundary;
	size_t start = headers["Content-Type"].find("boundary");
	if (start != std::string::npos){
		boundary = headers["Content-Type"].substr(start + 9, headers["Content-Type"].find("\r", start + 9));
		std::string tmp;
		if (headers["Transfer-Encoding"].find("chunked") == std::string::npos){
			size_t bodyStart = 0;
			while (true)
			{				
				size_t headerStart = body.find(boundary , bodyStart) + boundary.size();
				size_t fileStart = body.find("\r\n\r\n", bodyStart) + 4;
				tmp = body.substr(headerStart, fileStart - headerStart);
				size_t filenameStart = tmp.find("filename") + 10;
				std::string filename = tmp.substr(filenameStart, tmp.find("\"", filenameStart) - filenameStart);
				std::ofstream file (filename);
				size_t fileSize = body.find(boundary, fileStart);
				if (body[fileSize + boundary.size()] == '-'){
					file << body.substr(fileStart, fileSize - fileStart - 4);
					break;
				}
				file << body.substr(fileStart, fileSize - fileStart - 4);
				bodyStart = fileSize;
			}
		}
		else{
			
		}
	}
	//
	std::cout << "--------------------------------------------- BODY START\n";
	std::cout << body << "\n--------------------------------------------- BODY END\n";
		exit(0);
}

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
		headers[tmp.substr(0, end)] = tmp.substr(end + 2, tmp.size() - (end + 3));
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
	
	body = rawReq.substr(start+ 4, rawReq.size() - start);
	parseBody();
	
	// if (checkPath()){
	// }
	// if (checkHeaders()){

	// }
}