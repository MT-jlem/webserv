#include <regex>
#include <sys/_types/_s_ifmt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <iostream>
#include <string>
#include <cstdio>
#include <sys/_types/_size_t.h>
#include <algorithm>
#include <utility>
extern std::string err;
extern std::map<std::string, std::string> statusCodes;

//❌ to-do change error handling and err res
bool checkReDirPath(std::string path){
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
	return false;
}

Response::Response(){}
Response::Response(request &req, server &serv){
	pos = std::string::npos;
	res = "HTTP/1.1 ";
	locIndex = getLocation(req, serv);
	if (locIndex == -1){
		std::cout << "LOC not found\n";
		err = "404";
	} else {
		path = getPath(req, serv);
		file = getFile(serv);
	}
	// std::cout << serv.loc[locIndex].path << "<<\n";
	// std::cout << path << "<< path\n";
	// std::cout << file << "<< file \n";
	//❌ check if method is allowed in the location
	if (err != "")
		errorRes(serv, req);
	else if(req.getMethod() == "GET" ){
		if (serv.loc[locIndex].methods[GET]){
			if (!serv.loc[locIndex].redir.first.empty())
				reDirRes(serv, req);
			else 
				getM(serv, req);
		}
		else{
			err = "405";
			errorRes(serv, req);
		}
	}
	else if (req.getMethod() == "POST"){
		if (serv.loc[locIndex].methods[POST]){
			if (!serv.loc[locIndex].redir.first.empty())
				reDirRes(serv, req);
			else
				postM(req);
		}
		else{
			err = "405";
			errorRes(serv, req);
		}
	}
	else{
		if (serv.loc[locIndex].methods[POST]){
			if (!serv.loc[locIndex].redir.first.empty())
				reDirRes(serv, req);
			else
				deleteM();
		}
		else{
			err = "405";
			errorRes(serv, req);
		}
	}
}

Response::~Response(){}

void Response::reDirRes(server &serv, request &req){
	std::pair<std::string, std::string> tmp = serv.loc[locIndex].redir;
	if (checkReDirPath(tmp.second)){
		err = "500";
		errorRes(serv, req);
	} else {
		res += tmp.first;
		res += statusCodes[tmp.first];
		res += getDate();
		res += "server: webServ\r\n";
		res += "Content-Length: 0\r\n";
		res += "Location: "; res += tmp.second; res += "\r\n";
		res += "\r\n";
	}
	
}

std::string Response::getPath(request &req, server &serv){
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


std::string Response::getFile(server &serv){
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

int Response::getLocation(request &req, server &serv){
	size_t vecSize = serv.loc.size();
	std::string str = req.getPath();

	while (true) {
		for (size_t i = 0; i < vecSize; ++i){
			if (str == serv.loc[i].path)
				return i;
		}
		pos = str.rfind("/", pos-1);
		if (pos == std::string::npos)
			return  -1;
		str = str.substr(0, pos + 1);
	}
	return -1;
}

void Response::getM(server &serv, request &req){
	body = getBody(path, serv, req);
	if (err!= ""){
		std::cout << "wahasaaaan\n";
		errorRes(serv, req);
	}
	
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
	if (req.getHeader()["Content-Type"].find("multipart") != std::string::npos && start != std::string::npos){
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
	// for (size_t i = 0; i < data.size(); ++i)
	// 	std::cout << data[i] << "<<line\n";
	res += "200 ";
	res += statusCodes["200"];
	res += getHeaders();
	res += "\r\n\r\n";
}

void Response::deleteM(){
	if (remove(path.data())){
		std::cout << "ERROR\n";
		err = "";
		exit(1);
	}
	res += "204 ";
	res += statusCodes["204"];
	res += getDate();
	res += "\r\n";
}

// void Response::resBuilder(request &req, server &serv){}

// std::string Response::getResLine(request &req, server &serv){}

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
	str += "server: webServ\r\n";
	str += getContentLength();
	str += "Content-Type: ";
	if (err != "")
		str += "text/html\r\n";
	else
		str += getContentType(); str += "\r\n";
	return str;
}

std::string Response::getBody(const std::string &path, server &serv, request &req){
	std::string buff;
	std::string tmp;
	struct stat st;
	stat(path.data(),  &st);
	if ((st.st_mode & S_IFMT) == S_IFDIR){
		if (!serv.loc[locIndex].autoIndex){
			err = "403";
			return "";
		}
		std::string str = generateDirHtml(path, req);
		file = ".html";
		return str;
	}
	// use access to check for permission
	// use state to check if path is dir
	std::ifstream file(path);
	if (!file.is_open()){
		err = "404";
		return "";
	}
	while (getline(file, tmp)){
		buff += tmp;
		buff += "\n";
	}
	return buff;
}

void	 Response::errorRes(server &serv, request &req){

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
	body = getBody(path, serv,req);
	res += getHeaders();
	res += "\r\n";
	res += body;
	res += "\r\n";
}

std::string Response::generateDirHtml(std::string path, request &req){
	DIR *dir;
	struct dirent *dirFile;
	std::string str;
	dir = opendir(path.data());
	std::string tmpFile = req.getPath();
	
	str +=	"<!DOCTYPE html>\n<html>\n<head>\n<style>\nbody { background: lightgray; }\n"
			"li { font-size: 1.2em; margin: 10px; }\n</style>\n<title>Directory</title>\n</head>\n<body>\n";
	str += "<h1>";
	str += "directory list";
	str += "</h1>\n<ul>\n";
	path = path[path.size() - 1] != '/' ? path + "/" : path;
	if(!dir)
		return str;
	while ((dirFile = readdir(dir))) {
	str += "<li><a href=\"";
	tmpFile = tmpFile[tmpFile.size() -1] != '/' ? tmpFile + "/" : tmpFile;
	str += tmpFile + dirFile->d_name;//replace
	str += "\">";
	str += dirFile->d_name;//replace
	str += "</a></li>\n";
		
	}
	str += "</ul>\n</body>\n</html>";
	return str;
}

std::string Response::generateErrHtml(){
	std::string str;

	str += "<!DOCTYPE html>\n<html>\n<head>\n<title>Error Page</title>\n</head>\n";
	str += "<body>\n<div style=\"border: 5px solid black; position: "
			"absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); "
			"padding: 10px;\">\n<p style=\"font-size: larger;\"> ";
	str += err; str += ' '; str += statusCodes[err];
	str += "</p>\n</div>\n</body>\n</html>\n";
	return str;
}

