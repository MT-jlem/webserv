#include "request.hpp"
#include <vector>

request::request(const std::string &req){
	rawReq = req;
}

request::~request(){}

std::string 						request::getMethod(){

}
std::string 						request::getPath(){

}
std::string 						request::getVersion(){

}
std::map<std::string, std::string>	request::getHeader(){

}
std::string							request::getBody(){

}
bool								request::checkVerion(){

}
bool								request::checkPath(){

}
bool 								request::checkMethod(){

}
bool								request::checkHeaders(){

}

void	request::parse(std::string &body){

	std::vector<std::string> vec;
	size_t i = 0;
	size_t reqSize = rawReq.size();
	std::string	tmp;
	while (i < reqSize){
		++i;	
	}

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
