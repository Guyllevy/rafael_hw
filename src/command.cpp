
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "../include/command.hpp"


int read_commands(std::string path, std::vector<Command>& commands){
    std::ifstream file(path);
    std::stringstream buffer;
    std::string line;
    Command c;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            iss >> c.time >> c.uav_sn >> c.x >> c.y;
            commands.push_back(c);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }

    return 0;
}

void Command::show(){
        std::cout << "By Value - Serial Number: " << this->uav_sn 
                  << ", Time: " << this->time 
                  << ", X: " << this->x 
                  << ", Y: " << this->y << std::endl;
    }