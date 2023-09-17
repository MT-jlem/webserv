#include <climits>
#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "request.hpp"

std::string err = "";
std::map<std::string, std::string> statusCodes;

