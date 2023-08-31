#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iterator>
#include <string>
#include <map>
#include <sys/_types/_size_t.h>
#include <vector>
#include "request.hpp"
#include "server.hpp"
class Response {

private:
	std::string status;
	std::string code;
	std::map<std::string, std::string> headers;
	std::string body;
	int locIndex;
	std::string path;
	std::string file;
	std::vector<std::string> data;
	size_t pos;
public:
	std::string res;
	Response();
	Response(request &req, Server &serv);
	~Response();
	void	getM(Server &serv, request &req);
	void	postM(request &req);
	void	deleteM();
	void	resBuilder(request &req, Server &serv);
	void	errorRes(Server &serv, request &req);
	std::string getResLine(request &req, Server &serv);
	std::string getHeaders();
	std::string getBody(const std::string &path, Server &serv, request &req);
	std::string getDate();
	std::string getContentLength();
	std::string getContentType();
	std::string getPath(request &req, Server &serv);
	int getLocation(request &req, Server &serv);
	std::string getFile(Server &serv);
	std::string generateDirHtml(std::string path, request &req);
	std::string generateErrHtml();
	void reDirRes(Server &serv, request &req);
};

#endif