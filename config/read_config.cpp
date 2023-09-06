#include "read_config.hpp"

ReadConfig::ReadConfig(int ac, char *av) : _ac(0), _av(0)
{
	_ac = ac;
	_av = av;
}

ReadConfig::~ReadConfig(){
}

void ReadConfig::readingFile()
{
	std::ifstream myfile;
	std::string newString;
	if (_ac == 2)
	{
		myfile.open(_av);
		if (!myfile.is_open())
		{
			std::cout << "Error: File not found" << std::endl;
			exit (1);
		}
		getline(myfile, _configFile, '\0');
		// std::cout << _configFile << std::endl;
	}
	else if (_ac == 1)
		std::cout << "default config file here\n";
	else
	{
		std::cout << "Wrong arg number\n";
		exit (1);
	}
}

void ReadConfig::checkBraces()
{
	std::stack<char> s;
	int j = 0;
	bool isBraces = false;
	int vindx = 0;
	for (int i = 0; i < (int)_configFile.size(); i++) 
	{
		char c = _configFile[i];
		if (c == '{')
		{
			isBraces = true;
			s.push(c);
		}
		else if (c == '}')
		{
			if (s.empty() || s.top() != '{') 
			{
				std::cout << "Error: Braces are not well placed\n";
				exit(1);
			}
			s.pop();
		}
		if (s.empty() && isBraces)
		{
			isBraces = false;
			// _configFile = _configFile + "}";
			_serverBlocks.push_back(_configFile.substr(j, i));
			_serverBlocks[vindx].append("}");
			vindx++;
			j = i+1;
		}
	}
	if (!s.empty())
	{
		std::cout << "Error: Braces are not well placed\n";
		exit(1);
	}
	std::cout << "_serverBlocks.size() = " << _serverBlocks.size() << std::endl;

	// print the vector of server blocks

	// for(int i = 0; i < (int)_serverBlocks.size(); i++) 
	// {
	//     std::cout << "-------------------------------------\n";
	//     for(int j = 0; j < (int)_serverBlocks[i].size(); j++)
	//     {
	//         std::cout << _serverBlocks[i][j] << std::endl;
	//     }\

	// }

	// print the vector of server blocks
}

void    ReadConfig::parseServerBlocks()
{
	
}
