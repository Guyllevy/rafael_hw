#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <string>
#include <iostream>

struct Command{

    double time;
    int uav_sn;
    double x;
    double y;

    void show();
};

int read_commands(std::string path, std::vector<Command>& commands);


#endif