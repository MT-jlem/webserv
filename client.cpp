#include "client.hpp"

Client::Client(const Client &obj){
    // std::cout << "copy con\n";
    fd = obj.fd;
    read = obj.read;
    chunked = obj.chunked;
    content_length = obj.content_length;
    req = obj.req;
    firstTime = obj.firstTime;
    servIndex = obj.servIndex;
    processing = obj.processing;
    sendLength = obj.sendLength;
}

Client::Client(){
    sendLength = 0;
    processing = false;
    fd = -1;
    read = 0;
    chunked = false;
    content_length = 0;
    req = "";
    firstTime = true;
    servIndex = -1;
    __response = "";
}

Client::~Client(){};
