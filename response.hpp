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
	Response(request &req, server &serv);
	~Response();
	void	getM(server &serv, request &req);
	void	postM(server &serv, request &req);	
	void	deleteM();
	void	resBuilder(request &req, server &serv);
	void	errorRes(server &serv, request &req);
	std::string getResLine(request &req, server &serv);
	std::string getHeaders();
	std::string getBody(const std::string &path, server &serv, request &req);
	std::string getDate();
	std::string getContentLength();
	std::string getContentType();
	std::string getPath(request &req, server &serv);
	int getLocation(request &req, server &serv);
	std::string getFile(server &serv);
	std::string generateDirHtml(std::string path, request &req);
	std::string generateErrHtml();
	void reDirRes(server &serv, request &req);
};

#endif