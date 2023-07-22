#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
class request
{
private:
	std::string	rawReq;
	std::string method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;
	//std::vector<std::pair<std::string, std::string> > headers;
	std::string body;
public:
	request(const std::string &req);
	~request();

	void parse();
	std::string 						getMethod();
	std::string 						getPath();
	std::string 						getVersion();
	std::map<std::string, std::string>	getHeader();
	std::string							getBody();
	size_t								parseHeaders(size_t start);
	bool								checkVerion();
	bool								checkPath();
	bool 								checkMethod();
	bool								checkHeaders();
};




#endif