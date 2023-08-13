#include <iostream>
#include <sstream>
#include <string>

int main (){
	std::string path("abcdefjklmn");
	for (size_t i = 0; path[i]; ++i){
		std::cout << i << "<< path index \n";
		if ((path[i] >= 'A' && path[i] <= 'Z') || (path[i] >= 'a' && path[i] <= 'z') || (path[i] >= '0' && path[i] <= '9'))
			continue; //400
		if (path[i] == '~' || path[i] == '!' || (path[i] >= '#' && path[i] <= '/') || path[i] == ':' || path[i] == ';' || path[i] == '=' || path[i] == '?' || path[i] == '@')
			continue;
		if (path[i] == '[' || path[i] == ']' || path[i] == '_')
			continue;
		std::cout << "invalid\n"; // 400
	}
}




