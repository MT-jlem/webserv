// junky parsing
#include "request.hpp"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <map>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "server.hpp"

extern std::string err;
std::map<std::string, std::string> encode;
size_t toHex(std::string hex){
	std::stringstream tmp;
	size_t res;
	tmp << std::hex << hex;
	tmp >> res;
	return res;
}
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
	if (err != "")
		return;
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
std::multimap<std::string, std::string>&	request::getHeader(){
	return headers;
}
std::string							request::getBody(){
	return body;
}

std::string							request::getQuery(){
	return query;
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
			return true;
	}
	size_t start = path.find("?");
	if (start != path.npos){
		query = path.substr(start + 1, path.size() - (start + 1));
		path = path.substr(0, start);
	}
	return false;
}

bool 								request::checkMethod(){
	if (method != "GET" && method != "POST" && method != "DELETE"){
		err = "501";
		return true;
	}
	return false;
}

bool								request::checkHeaders(){  
	if (headers.find("Transfer-Encoding") != headers.end()){
		if (headers.find("Transfer-Encoding")->second.find("chunked") == std::string::npos){
			err = "501";
			return true;
		}
	}

	if (method == "POST"){
		if (headers.find("Content-Length") == headers.end() && headers.find("Transfer-Encoding") == headers.end()){
			err = "400";
			return true;
		}
	}
	return false;
}

void	request::parseBody(){
	std::string boundary;
	std::multimap<std::string, std::string>::iterator it =  headers.find("Content-Type");

	if (it == headers.end())
			return;
	std::string header = it->second;
	size_t start = header.find("boundary");
	if (start != std::string::npos){
		boundary = header.substr(start + 9, header.find("\r", start + 9));
		it = headers.find("Transfer-Encoding");
		if (it == headers.end())
			return;
		header = it->second;
		if (header.find("chunked") != std::string::npos){
			std::string tmpBody = "";
			std::string tmp;
			long long chunk = 0;
			size_t start = 0;
			long long end =0;

			while (1) {
				start = body.find("\r\n", end);
				if (start == std::string::npos){
					err = "500";
					return;
				}
				tmp = body.substr(end, start - end);
				chunk = toHex(tmp.c_str());
				if (chunk == 0)
					break;
				tmpBody.append(body.substr(start + 2, chunk));//
				end += chunk + tmp.size() + 4;
			}
			body =  tmpBody;
		}
	}
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
		headers.insert(std::make_pair(tmp.substr(0, end) , tmp.substr(end + 2, tmp.size() - (end + 3))));
	}
	return ret;
}

void	request::parse(Server &serv){
	if (err != "")
		return;
	try {
		size_t i = 0, start = 0;
		std::string	tmp;

		i = rawReq.find(" ", 0);
		if (i == rawReq.npos){
			err = "400";
			return;
		}
		method = rawReq.substr(0, i);
		start = rawReq.find(" ", i + 1);
		if (start == rawReq.npos){
			err = "400";
			return;
		}
		path = rawReq.substr(i + 1, start - i - 1);
		i = rawReq.find("\r", start + 1);
		if (i == rawReq.npos){
			err = "400";
			return;
		}
		version = rawReq.substr(start + 1, i - start - 1);
		if (checkVerion())
			return;
		if (checkMethod())
			return;
		if (checkPath())
			return;
		urlDecoding();
		start = parseHeaders(i + 2);
		if (checkHeaders())
			return;
		body = rawReq.substr(start+ 4, rawReq.size() - start);
		if(body.size() > serv.maxBodySize){
			err = "413";
			return;
		}
		parseBody();
	} catch (std::exception &) {
		err = "500";
		return;
	}
}