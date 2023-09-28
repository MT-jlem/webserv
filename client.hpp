#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
	public:
		int fd;
		long long read;
		bool chunked;
		long long content_length;
		std::string req;
		bool firstTime;
		int servIndex;
		bool processing;
		std::string __response;
		long long sendLength;
		Client(const Client &obj);
		Client();
		~Client();
};

#endif