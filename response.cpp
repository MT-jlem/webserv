#include "response.hpp"
#include "server.hpp"
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
extern std::string err;
extern std::map<std::string, std::string> statusCodes;

//❌ to-do change error handling and err res

Response::Response(){}
Response::Response(request &req, Server &serv){
	pos = std::string::npos;
	res = "HTTP/1.1 ";
	locIndex = getLocation(req, serv);
	if (locIndex == -1){
		std::cout << "LOC not found\n";
		exit(0);
	}
	// std::cout << serv.loc[locIndex].path << "<<\n";
	path = getPath(req, serv);
	// std::cout << path << "<< path\n";
	file = getFile(serv);
	// std::cout << file << "<< file \n";
	if (err != "")
		errorRes(serv);
	else if(req.getMethod() == "GET")
		getM(serv);
	else if (req.getMethod() == "POST")
		postM(req);
	else
		deleteM(req, serv);
}

Response::~Response(){}

std::string Response::getPath(request &req, Server &serv){
	std::string root = serv.loc[locIndex].root != "" ? serv.loc[locIndex].root : serv.root;
	std::string reqPath = req.getPath();
	if (pos != std::string::npos)
		file = reqPath.substr(pos + 1, reqPath.size() - pos + 1);
		// if (serv.loc[locIndex].errorPage.second.find(err) != serv.loc[locIndex].errorPage.second.end())
		//check if there a custom error page if not generate the default
	if (err != "")
		return "";
	return  root + file;
}

std::string Response::getFile(Server &serv){
	if (err != "")
		return "";
	if (file != "")
		return file;
	if (serv.loc[locIndex].index != ""){
		path += serv.loc[locIndex].index;
		return serv.loc[locIndex].index;
	}
	if (serv.index != ""){
		path += serv.index;
		return serv.index;
	}
	return "";
}

int Response::getLocation(request &req, Server &serv){
	size_t vecSize = serv.loc.size();
	std::string str = req.getPath();

	while (true) {
		for (size_t i = 0; i < vecSize; ++i){
			if (str == serv.loc[i].path)
				return i;
		}
		pos = str.rfind("/");
		if (pos == str.npos)
			return  -1;
		str = str.substr(0, pos + 1);
	}
	return -1;
}

void Response::getM(Server &serv){
	body = getBody(path);
	if (res != err)
		errorRes(serv);
	res += "200 ";
	res += statusCodes["200"];
	res += getHeaders();
	res += "\r\n";
	res += body;
	res += "\r\n";
}

void Response::postM(request &req){
	std::string tmp;
	std::string boundary;
	std::string tmpData = req.getBody();
	size_t bodyStart = 0;
	size_t start = req.getHeader()["Content-Type"].find("boundary");
	if (req.getHeader()["Content-Type"].find("multipart/form-data") != std::string::npos && start != std::string::npos){
		boundary = req.getHeader()["Content-Type"].substr(start + 9, req.getHeader()["Content-Type"].find("\r", start + 9));
		while (true)
		{				
			size_t headerStart = tmpData.find(boundary , bodyStart) + boundary.size();
			size_t fileStart = tmpData.find("\r\n\r\n", bodyStart) + 4;
			tmp = tmpData.substr(headerStart, fileStart - headerStart);
			size_t filenameStart = tmp.find("filename");
			size_t fileSize = tmpData.find(boundary, fileStart);
			if (filenameStart != tmp.npos)
			{
				filenameStart += 10;
				std::string filename = tmp.substr(filenameStart, tmp.find("\"", filenameStart) - filenameStart);
				std::ofstream file (filename);
				if (tmpData[fileSize + boundary.size()] == '-'){
					file << tmpData.substr(fileStart, fileSize - fileStart - 4);
					break;
				}
				file << tmpData.substr(fileStart, fileSize - fileStart - 4);
			} else {
				if (tmpData[fileSize + boundary.size()] == '-'){
					data.push_back(tmpData.substr(fileStart, fileSize - fileStart - 4));
					break;
				}
				data.push_back(tmpData.substr(fileStart, fileSize - fileStart - 4));
			}
			bodyStart = fileSize;
		}
	} else if (req.getHeader()["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos){
		data.push_back(tmpData);	
	}	else
		err = "501";
	for (size_t i = 0; i < data.size(); ++i)
		std::cout << data[i] << "<<line\n";
	res += "200 ";
	res += statusCodes["200"];
	res += getHeaders();
	res += "\r\n\r\n";
}

void Response::deleteM(request &req, Server &serv){
	std::cout << req.getPath();
	std::cout << serv.index;
}

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

std::string Response::getContentType(){
	std::string str = "text/plain";
    if (file.rfind('.') != std::string::npos) {
        std::string ext = file.substr(file.rfind('.'), file.size());
        if (ext == ".html")
            str = "text/html";
        else if (ext == ".js")
            str = "application/javascript";
        else if (ext == ".css")
            str = "text/css";
        else if (ext == ".ico")
            str = "image/x-icon";
        else if (ext == ".jpeg" || ext == ".jpg")
            str = "image/jpeg";
        else if (ext == ".png")
            str = "image/png";
        else if (ext == ".mp4")
            str = "video/mp4";
        else if (ext == ".gif")
            str = "image/gif";
        else if (ext == ".tiff")
            str = "image/tiff";
        else if (ext == ".svg")
            str = "image/svg+xml";
        else if (ext == ".csv")
            str = "text/csv";
        else if (ext == ".xml")
            str = "application/xml";
        else if (ext == ".zip")
            str = "application/zip";
        else if (ext == ".mp3")
            str = "audio/mpeg";
        else if (ext == ".wma")
            str = "audio/x-ms-wma";
        else if (ext == ".ra")
            str = "audio/vnd.rn-realaudio";
        else if (ext == ".wav")
            str = "audio/x-wav";
        else if (ext == ".mpeg")
            str = "video/mpeg";
        else if (ext == ".mov")
            str = "video/quicktime";
        else if (ext == ".m4v")
            str = "video/x-m4v";
        else if (ext == ".qt")
            str = "video/quicktime";
        else if (ext == ".wmv")
            str = "video/x-ms-wmv";
        else if (ext == ".avi")
            str = "video/x-msvideo";
        else if (ext == ".flv")
            str = "video/x-flv";
        else if (ext == ".webm")
            str = "video/webm";
        else if (ext == ".odt")
            str = "application/vnd.oasis.opendocument.text";
        else if (ext == ".ods")
            str = "application/vnd.oasis.opendocument.spreadsheet";
        else if (ext == ".odp")
            str = "application/vnd.oasis.opendocument.presentation";
        else if (ext == ".odg")
            str = "application/vnd.oasis.opendocument.graphics";
        else if (ext == ".ods")
            str = "application/vnd.oasis.opendocument.spreadsheet";
        else if (ext == ".odt")
            str = "application/vnd.oasis.opendocument.text";
        else if (ext == ".ppt")
            str = "application/vnd.ms-powerpoint";
        else if (ext == ".pptx")
            str = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        else if (ext == ".xls")
            str = "application/vnd.ms-excel";
        else if (ext == ".xlsx")
            str = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        else if (ext == ".doc")
            str = "application/msword";
        else if (ext == ".docx")
            str = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        else if (ext == ".xul")
            str = "application/vnd.mozilla.xul+xml";
    }
    return str;
}


std::string Response::getHeaders(){
	std::string str;
	str += getDate();
	str += "Server: webServ\r\n";
	str += getContentLength();
	str += "Content-Type: ";
	if (err != "")
		str += "text/html\r\n";
	else
		str += getContentType(); str += "\r\n";
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

void	 Response::errorRes(Server &serv){
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
	res += getHeaders();
	res += "\r\n";
	res += body;
	res += "\r\n";
}

