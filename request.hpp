#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <map>
#include "server.hpp"
#include "config/conf.hpp"
class request
{
private:
	std::string method;
	std::string path;
	std::string version;
	std::string query;
	std::multimap<std::string, std::string> headers;
	//std::vector<std::pair<std::string, std::string> > headers;
	std::string body;
public:
	std::string	rawReq;
	request(const std::string &req);
	~request();

	void parse(Server &serv);
	std::string							getQuery();
	std::string 						getMethod();
	std::string 						getPath();
	std::string 						getVersion();
	std::multimap<std::string, std::string>&	getHeader();
	std::string							getBody();
	size_t								parseHeaders(size_t start);
	void								urlDecoding();
	void								parseBody();
	bool								checkVerion();
	bool								checkPath();
	bool 								checkMethod();
	bool								checkHeaders();

};




#endif