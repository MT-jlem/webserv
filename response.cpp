#include <cstddef>
#include <cstring>
#include <map>
#include <regex>
#include <stdexcept>
#include <strings.h>
#include <sys/_types/_s_ifmt.h>
#include <sys/fcntl.h>
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
#include <sys/unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include "config/conf.hpp"
extern std::string err;
extern std::map<std::string, std::string> statusCodes;

bool checkReDirPath(std::string path){
	if (path.size() > 2048){
		err = "414";
		return true;
	}
	for (size_t i = 0; path[i]; ++i){
		if ((path[i] >= 'A' && path[i] <= 'Z') || (path[i] >= 'a' && path[i] <= 'z') ||
		 (path[i] >= '0' && path[i] <= '9'))
			continue;
		if (path[i] == '~' || path[i] == '!' || (path[i] >= '#' && path[i] <= '/') ||
			path[i] == ':' || path[i] == ';' || path[i] == '=' || path[i] == '?' || path[i] == '@')
			continue;
		if (path[i] == '[' || path[i] == ']' || path[i] == '_')
			continue;
		err = "400";
		return true;
	}
	return false;
}

Response::Response(){}
Response::Response(request &req, Server &serv){
	res = "HTTP/1.1 ";
	locIndex = -1;
	pos = req.getPath().size() - 1;
	errorHandled = false;

	if (err != ""){
		std::cout << req.rawReq << '\n';
		std::cout << "const\n";
		errorRes(serv, req);
		return;
	}
	locIndex = getLocation(req, serv);
	if (locIndex == -1){
		err = "404";
		return;
	} else {
		path = getPath(req, serv);
		file = getFile(serv);
		if (file != ""){
			size_t pos = file.rfind(".");
			pos = pos != file.npos ? pos : file.size() -1;
			ext = file.substr(pos, file.size());
		}
	}
}

Response::~Response(){}

void Response::reDirRes(Server &serv, request &req){
	std::pair<std::string, std::string> tmp = serv.loc[locIndex].redir;
	if (checkReDirPath(tmp.second)){
		err = "500";
		errorRes(serv, req);
		return;
	} else {
		res += tmp.first;
		res += statusCodes[tmp.first];
		res += getDate();
		res += "Server: webServ(liban lik)\r\n";
		res += "Connection: close\r\n";
		res += "Content-Length: 0\r\n";
		res += "Location: "; res += tmp.second; res += "\r\n";
		res += "\r\n";
	}
}

std::string Response::getPath(request &req, Server &serv){
	std::string root = serv.loc[locIndex].root != "" ? serv.loc[locIndex].root : serv.root;
	std::string reqPath = req.getPath();

	root = root[root.size() - 1] != '/' ? root + "/" : root;
	if (pos != std::string::npos)
		file = reqPath.substr(pos + 1, reqPath.size() - pos + 1);
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

	std::cout << vecSize << "<< loc size\n";
	std::cout << str << "<< req path\n";
	while (true) {
		for (size_t i = 0; i < vecSize; ++i){
			if (str == serv.loc[i].path)
				return i;
		}
		pos = str.rfind("/", pos-1);
		if (pos == std::string::npos){
			std::cout << str << '\n';
			err = "400";
			return  -1;
		}
		str = str.substr(0, pos + 1);
	}
	return -1;
}

void Response::getM(Server &serv, request &req){
	body = getBody(path, serv, req);
	if (err!= ""){
		errorRes(serv, req);
		return;
	}
	res += "200 ";
	res += statusCodes["200"];
	res += getHeaders();
	res += "\r\n";
	res += body;
	res += "\r\n";
}

void Response::postM(Server &serv, request &req){
	std::string tmp;
	std::string boundary;
	std::string tmpData = req.getBody();
	size_t bodyStart = 0;
	if (!tmpData.empty()){
		std::multimap<std::string, std::string>::iterator it =  req.getHeader().find("Content-Type");
		size_t start = it->second.find("boundary");
		if (req.getHeader().find("Content-Type")->second.find("multipart") != std::string::npos && start != std::string::npos){
			boundary = req.getHeader().find("Content-Type")->second.substr(start + 9, req.getHeader().find("Content-Type")->second.find("\r", start + 9));
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
					// path = path[path.size() -1 ] != '/' ? path + "/" : path;
					std::string upload = serv.loc[locIndex].upload != "" ? serv.loc[locIndex].upload : "/tmp/";
					std::ofstream file (upload + filename);
					if (!file.is_open() || file.fail()){
						std::cout << "can't upload(file not created)\n";
						err = "500";
						return;
					}
					if (tmpData[fileSize + boundary.size()] == '-'){
						file << tmpData.substr(fileStart, fileSize - fileStart - 4);
						file.close();
						break;
					}
					file << tmpData.substr(fileStart, fileSize - fileStart - 4);
					file.close();
				} else {
					if (tmpData[fileSize + boundary.size()] == '-'){
						data.push_back(tmpData.substr(fileStart, fileSize - fileStart - 4));
						break;
					}
					data.push_back(tmpData.substr(fileStart, fileSize - fileStart - 4));
				}
				bodyStart = fileSize;
			}
		} else if (req.getHeader().find("Content-Type")->second.find("application/x-www-form-urlencoded") != std::string::npos){
			data.push_back(tmpData);	
		}	else{
			data.push_back(tmpData);
		}
		if (!serv.loc[locIndex].cgiPath.empty() && (ext == ".py" || ext == ".php")){
			body = execCgi(serv, req);
			file = ".html";
			res += "200";
			res += statusCodes["200"];
		}else{
			res += "204 ";
			res += statusCodes["204"];
		}
		res += getHeaders();
		res += "\r\n";
		res += body; res += "\r\n";
	}
	else {
		getM(serv, req);
	}
}

void Response::deleteM(){
	if (remove(path.data())){
		std::cout << "delete m error \n";
		err = "500";
		return;
	}
	res += "204 ";
	res += statusCodes["204"];
	res += getDate();
	res += "\r\n";
}

void Response::resBuilder(request &req, Server &serv){
	
	if (err != ""){
		errorRes(serv, req);
		return;
	}
	else if(req.getMethod() == "GET" ){
		if (serv.loc[locIndex].methods[GET]){
			if (!serv.loc[locIndex].redir.first.empty())
				reDirRes(serv, req);
			else 
				getM(serv, req);
			if (err != ""){
					errorRes(serv, req);
					return;
				}
		}
		else{
			err = "405";
			errorRes(serv, req);
			return;
		}
	}
	else if (req.getMethod() == "POST"){
		if (serv.loc[locIndex].methods[POST]){
			if (!serv.loc[locIndex].redir.first.empty())
				reDirRes(serv, req);
			else
				postM(serv, req);
		}
		else{
			err = "405";
			errorRes(serv, req);
			return;
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
			return;
		}
	}
}

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
	str += "Server: webServ(liban lik)\r\n";
	str += "Connection: close\r\n";
	str += getContentLength();
	str += "Content-Type: ";
	if (err != "")
		str += "text/html";
	else
		str += getContentType();
	str += "\r\n";
	return str;
}

std::string Response:: getBody(const std::string &path, Server &serv, request &req){
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

	if (access(path.data(), F_OK)){
		err = "404";
		return "";
	} else if (access(path.data(), R_OK)){
		err = "403";
		return "";
	}
	if (!serv.loc[locIndex].cgiPath.empty() && (ext == ".py" || ext == ".php")){
		buff = execCgi(serv, req);
		file = ".html";
	}else {
		std::ifstream file(path);
		if (!file.is_open()){
			err = "500";
			return "";
		}
		while (getline(file, tmp)){
			buff += tmp;
			buff += "\n";
		}
		file.close();
	}
	return buff;
}

void	 Response::errorRes(Server &serv, request &req){
	if (errorHandled)
		return;
	body = "";
	res = "HTTP/1.1 ";
	if (locIndex >= 0){
		if (serv.loc[locIndex].errorPage[err] != ""){
			body = getBody(serv.loc[locIndex].errorPage[err], serv,req);
		}
	}
	else if (serv.errorPage[err] != "")
		body = getBody(serv.errorPage[err], serv,req);
	if (body == "")
		body = generateErrHtml();
	res += err;
	res += statusCodes[err];
	res += getHeaders();
	res += "\r\n";
	res += body;
	res += "\r\n";
	errorHandled = true;
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
	while ((dirFile = readdir(dir))) {
		str += "<li><a href=\"";
		tmpFile = tmpFile[tmpFile.size() -1] != '/' ? tmpFile + "/" : tmpFile;
		str += tmpFile + dirFile->d_name;//replace
		str += "\">";
		str += dirFile->d_name;//replace
		str += "</a></li>\n";

	}
	str += "</ul>\n</body>\n</html>";
	closedir(dir);
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

void Response::initializeEnv(request &req){
	std::string var[] = {"PATH_INFO=", "REQUEST_METHOD=", "QUERY_STRING=",
	"CONTENT_TYPE=", "CONTENT_LENGTH=","HTTP_COOKIE=", "SCRIPT_NAME=",
	"HTTP_USER_AGENT=", "SCRIPT_FILENAME=", "REDIRECT_STATUS="};
	std::multimap<std::string, std::string> headers = req.getHeader();
	std::multimap<std::string, std::string>::iterator it;
	var[0] += "";
	var[1] += req.getMethod();
	var[2] += req.getQuery();
	it = headers.find("Content-Type");
	var[3] += it != headers.end() ? it->second : "";
	it = headers.find("Content-Length");
	var[4] += it != headers.end() ? it->second : "";
	it = headers.find("Cookie");
	var[5] +=  it != headers.end() ? it->second : "";
	var[6] += req.getPath();
	it = headers.find("User-Agent");
	var[7] +=  it != headers.end() ? it->second : "";
	var[8] += path;
	var[9] += "200";
	for (int i = 0; i < 10; ++i){
		cgiEnv[i] = strdup(var[i].c_str());
	}
	cgiEnv[10] = NULL;

}

std::string Response::execCgi(Server &serv, request &req){

	initializeEnv(req);
	int status;
	int fd[2];
	int pid;
	char *arg[3];

	int cgiFile = open("/tmp/cgiFile", O_RDWR | O_CREAT | O_TRUNC, 0644);
	int cgiRes = open("/tmp/cgiRes", O_RDWR | O_CREAT | O_TRUNC, 0644);
	std::string str;
	if (cgiFile < 0 || cgiRes < 0){
		err = "404";
		close(cgiFile);
		close(cgiRes);
		std::cout << "execCgi failed\n";
		err = "500";
		return "";
	}
	if (req.getMethod() == "POST"){
		write(cgiFile, data[0].c_str(), data[0].size());
		lseek(cgiFile, 0, SEEK_SET);
		// close(cgiFile);
	}
	arg[0] = (char *)serv.loc[locIndex].cgiPath[ext].c_str();
	arg[1] = (char *)path.c_str();
	arg[2] = NULL;
	if (pipe(fd) < 0){
		err = "500";
		return "";
	}
	pid = fork();
	if (pid < 0){
		err = "500";
		return "";
	} else if (pid == 0){
		dup2(cgiFile, STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		execve(arg[0], arg, cgiEnv);
	} else{
		char buff[BUFFER_SIZE];
		waitpid(pid, &status, 0);
		close(fd[1]);
		while (true) {
			long long r = read(fd[0], buff, BUFFER_SIZE -1);
			if (r == 0){
				break;
			}
			else if (r < 0) {
				err = "500";
				return  "";
			}
			buff[r - 1] = '\0';
			str.append(buff, r);
			bzero(buff, r);
		
		}
		close(fd[0]);
	}
	close(cgiFile);
	close(cgiRes);
	return str;
}
