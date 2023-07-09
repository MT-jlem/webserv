#include "request.hpp"

request::request(const std::string &req){
	rawReq = req;
}

request::~request(){}

void	request::parse(){
	method = getMethod();
	path = getPath();
	version =  getVersion();
	headers = getHeader();
	body = getBody();

	if (checkVerion()){

	}
	if (checkPath()){

	}
	if (checkMethod()){
		//error
	}
	if (checkHeaders()){

	}
}