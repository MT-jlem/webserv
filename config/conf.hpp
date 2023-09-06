#ifndef CONF_HPP
#define CONF_HPP


#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "read_config.hpp"


class Conf : public ReadConfig
{
    private:
        
    public:
        Conf(int ac, char *av);
        ~Conf();
        void    fill_Directives_Locations();
        // void    
        // take for every serv a vec of loc
};


#endif