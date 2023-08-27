#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iterator>
#include <string>
#include <map>
#include <sys/_types/_size_t.h>
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
	size_t pos;
public:
	std::string res;
	Response();
	Response(request &req, Server &serv);
	~Response();
	void	getM();
	void	postM(request &req, Server &serv);
	void	deleteM(request &req, Server &serv);
	void	resBuilder(request &req, Server &serv);
	void	errorRes(Server &serv);
	std::string getResLine(request &req, Server &serv);
	std::string getHeaders();
	std::string getBody(const std::string &path);
	std::string getDate();
	std::string getContentLength();
	std::string getContentType();
	std::string getPath(request &req, Server &serv);
	int getLocation(request &req, Server &serv);
	std::string getFile(Server &serv);
};

#endif