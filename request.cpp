// junky parsing
#include "request.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "server.hpp"
extern std::string err;
std::map<std::string, std::string> encode;

// remove boundary + headers from the body 
// handle chunked req && convert size from hex to int and recv the body && remove the boundary
void initializeEncode(){
	encode["%20"] = " ";
	encode["%21"] = "!";
	encode["%22"] = "\"";
	encode["%23"] = "#";
	encode["%24"] = "$";
	encode["%25"] = "%";
	encode["%26"] = "&";
	encode["%27"] = "\'";
	encode["%28"] = "(";
	encode["%29"] = ")";
	encode["%2A"] = "*";
	encode["%2B"] = "+";
	encode["%2C"] = ",";
	encode["%2F"] = "/";
	encode["%3A"] = ":";
	encode["%3B"] = ";";
	encode["%3D"] = "=";
	encode["%3F"] = "?";
	encode["%40"] = "@";
	encode["%5B"] = "[";
	encode["%5D"] = "]";
}
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
std::map<std::string, std::string>	request::getHeader(){
	return headers;
}
std::string							request::getBody(){
	return body;
}
bool								request::checkVerion(){
	if(version != "HTTP/1.1"){
		err = "505";
		return true;
	}
	return false;
}
void								request::urlDecoding(){
	size_t start = 0;

	while ((start = path.find("%", start)) != path.npos) {
		path.replace(start, 3, encode[path.substr(start, 3)]);
	}

}
bool								request::checkPath(){
	if (path.size() > 2048)
		return true; //status code 414
	for (size_t i = 0; path[i]; ++i){
			if ((path[i] >= 'A' && path[i] <= 'Z') || (path[i] >= 'a' && path[i] <= 'z') || (path[i] >= '0' && path[i] <= '9'))
				continue; //400
			if (path[i] == '~' || path[i] == '!' || (path[i] >= '#' && path[i] <= '/') || path[i] == ':' || path[i] == ';' || path[i] == '=' || path[i] == '?' || path[i] == '@')
				continue;
			if (path[i] == '[' || path[i] == ']' || path[i] == '_')
				continue;
			err = "400";
			return true; // 400
	}
	size_t start = path.find("?");
	if (start != path.npos){
		query = path.substr(start + 1, path.size() - (start + 1));
		path = path.substr(0, start);
	}
	else
		query = "";
	// std::cout << "---------------------HERE---------------------\n";
	// separate the query from the url/path then check it && check for err
	return false;
}

bool 								request::checkMethod(){
	//check method is allowed 405
	if (method != "GET" && method != "POST" && method != "DELETE"){
		err = "501";
		return true;//501
	}
	return false;
}

bool								request::checkHeaders(){  
	if (headers.find("Transfer-Encoding") != headers.end()){
		if (headers["Transfer-Encoding"].find("chunked") == std::string::npos){
			err = "501";
			return true;//status code 501
		}
	}

	if (method == "POST"){
		if (headers.find("Content-Length") == headers.end() && headers.find("Transfer-Encoding") == headers.end()){
			err = "400";
			return true;//status code 400
		}
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
			// size_t bodyStart = 0;
			// while (true)
			// {				
			// 	size_t headerStart = body.find(boundary , bodyStart) + boundary.size();
			// 	size_t fileStart = body.find("\r\n\r\n", bodyStart) + 4;
			// 	tmp = body.substr(headerStart, fileStart - headerStart);
			// 	size_t filenameStart = tmp.find("filename") + 10;
			// 	std::string filename = tmp.substr(filenameStart, tmp.find("\"", filenameStart) - filenameStart);
			// 	std::ofstream file (filename);
			// 	size_t fileSize = body.find(boundary, fileStart);
			// 	if (body[fileSize + boundary.size()] == '-'){
			// 		file << body.substr(fileStart, fileSize - fileStart - 4);
			// 		break;
			// 	}
			// 	file << body.substr(fileStart, fileSize - fileStart - 4);
			// 	bodyStart = fileSize;
			// }
		}
		else{
			//remove the hex;
		}
	}
	// std::cout << "--------------------------------------------- BODY START\n";
	// std::cout << body << "\n--------------------------------------------- BODY END\n";
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
	// for ( std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	// 	std::cout << "line >> " << it->first << ":" << it->second << "\n";
	return ret;
}

void	request::parse(Server &serv){

	size_t i = 0, start = 0;
	std::string	tmp;

	i = rawReq.find(" ", 0);//error handling for find and substr
	if (i == rawReq.npos){
		err = "400";
		return;
	}
	method = rawReq.substr(0, i);
	// std::cout << method << "-METHOD\n";

	start = rawReq.find(" ", i + 1);
	if (start == rawReq.npos){
		err = "400";
		return;
	}
	path = rawReq.substr(i + 1, start - i - 1);
	// std::cout << path << "-PATH\n";

	i = rawReq.find("\r", start + 1);
	if (i == rawReq.npos){
		err = "400";
		return;
	}
	version = rawReq.substr(start + 1, i - start - 1);
	// std::cout << version << "-VERSION\n";
	//check for error
	if (checkVerion())
		return;
	if (checkMethod())
		return;
	if (checkPath())
		return;
	std::cout << path << " before \n";
	urlDecoding();
	std::cout << path << " after \n";
	start = parseHeaders(i + 2);
	if (checkHeaders())
		return;
	body = rawReq.substr(start+ 4, rawReq.size() - start);
	//IF body.size() > max body size in config file THEN return 413
	if(body.size() > serv.maxBodySize){
		err = "413";
		return;
	}
	parseBody();
}