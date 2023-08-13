#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
class Response {

private:
	std::string status;
	std::string code;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string res;
public:
	Response();
	~Response();
};

#endif