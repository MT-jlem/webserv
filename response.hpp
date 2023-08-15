#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include "request.hpp"
#include "server.hpp"
class Response {

private:
	std::string status;
	std::string code;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string res;
	size_t locIndex;
public:
	Response();
	Response(request &req, Server &serv);
	~Response();
	void getM(request &req, Server &serv);
	void postM(request &req, Server &serv);
	void deleteM(request &req, Server &serv);
	void resBuilder(request &req, Server &serv);
	std::string getResLine(request &req, Server &serv);
	std::string getHeaders(request &req, Server &serv);
	std::string getBody(const std::string &path);
	std::string getDate();
	std::string getContentLength();
	std::string getContentType();
	void	errorRes(request &req, Server &serv);
};

#endif