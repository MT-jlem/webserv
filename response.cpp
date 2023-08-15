#include "response.hpp"
#include <fstream>
#include <ctime>
#include <iostream>
extern std::string err;
extern std::map<std::string, std::string> statusCodes;

Response::Response(){}

Response::Response(request &req, Server &serv){
	res = "HTTP/1.1 ";
	if (err != "")
		errorRes(req, serv);
	else if(req.getMethod() == "GET")
		getM(req, serv);
	else if (req.getMethod() == "POST")
		postM(req, serv);
	else
		deleteM(req, serv);
}

Response::~Response(){}

// void Response::getM(request &req, Server &serv){

// }

// void Response::postM(request &req, Server &serv){

// }

// void Response::deleteM(request &req, Server &serv){

// }

// void Response::resBuilder(request &req, Server &serv){}

// std::string Response::getResLine(request &req, Server &serv){}

std::string Response::getDate(){
	std::tm *date;
	std::time_t now = time(nullptr);
	std::string str;

	date = gmtime(&now);
	str += "Date: ";
	switch (date->tm_wday){
		case 0: str += "Sun, "; break;
		case 1: str += "Mon, " ;break;
		case 2: str += "Tue, "; break;
		case 3: str += "Wed, "; break;
		case 4: str += "Thu, "; break;
		case 5: str += "Fri, "; break;
		case 6: str += "Sat, "; break;
		default: break;
	}
	str += std::to_string(date->tm_mday); str += " ";
	switch (date->tm_mon){
		case 0: str += "Jan "; break;
		case 1: str += "Feb " ;break;
		case 2: str += "Mar "; break;
		case 3: str += "Apr "; break;
		case 4: str += "May "; break;
		case 5: str += "Jun "; break;
		case 6: str += "Jul "; break;
		case 7: str += "Aug "; break;
		case 8: str += "Sep " ;break;
		case 9: str += "Oct "; break;
		case 10: str += "Nov "; break;
		case 11: str += "Dec "; break;
		default: break;
	}
	date->tm_year += 1900;
	str += std::to_string(date->tm_year); str += " ";
	str += std::to_string(date->tm_hour); str += ":";
	str += std::to_string(date->tm_min); str += ":";
	str += std::to_string(date->tm_sec); str += " ";
	str += "GMT\r\n";
	return str;
}

std::string Response::getContentLength(){
	std::string str("Content-Length: ");

	str += std::to_string(body.size());
	str += "\r\n";
	return str;
}

// std::string Response::getContentType(){

// }

std::string Response::getHeaders(request &req, Server &serv){
	std::string str;
	
	str += getDate();
	str += "Server: webserv\r\n";
	str += getContentLength();
	str += getContentType(/*fileName*/);

	return str;
}

std::string Response::getBody(const std::string &path){
	std::string buff;
	std::string tmp;
	std::ifstream file(path);
	
	while (getline(file, tmp)){
		buff += tmp;
		buff += "\n";
	}
	return buff;
}

void	 Response::errorRes(request &req, Server &serv){
	// if there's custom error page we should know which location the error occurred
	std::string path;
	if (serv.loc[locIndex].errorPage.second.find(err) != serv.loc[locIndex].errorPage.second.end())
		path = serv.loc[locIndex].errorPage.first;
	else{
		path = "./errorPages/";
		path += err;
		path += ".html";
	}
	res += err; res += " ";
	res += statusCodes[err]; res += " \r\n";
	body = getBody(path);
	res += getHeaders(req, serv);
}
