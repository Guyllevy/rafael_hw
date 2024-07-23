#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "init_data.hpp"

void InitData::show(){
    std::cout << "N_uav: " << N_uav << std::endl;
    std::cout << "Dt: " << Dt << std::endl;
    std::cout << "R: " << R << std::endl;
    std::cout << "X0: " << X0 << std::endl;
    std::cout << "Y0: " << Y0 << std::endl;
    std::cout << "Z0: " << Z0 << std::endl;
    std::cout << "V0: " << V0 << std::endl;
    std::cout << "Az: " << Az << std::endl;
    std::cout << "TimeLim: " << TimeLim << std::endl;
}

double parse_double(std::string content, std::string key) {
    std::regex re(key + R"(\s*=\s*(-)?\d+(\.\d+)?)");
    std::smatch match;
    int found = std::regex_search(content, match, re);
    std::string var_str = match[0].str().substr(match[0].str().find("=") + 1);

    if (var_str.empty()){
        std::cout << "failed to read parameter " << key << std::endl;
        return 1;
    }
    else {
        return atof(var_str.c_str());
    }
}

int parse_int(std::string content, std::string key) {
    std::regex re(key + R"(\s*=\s*\d+(\.\d+)?)");
    std::smatch match;
    int found = std::regex_search(content, match, re);
    std::string var_str = match[0].str().substr(match[0].str().find("=") + 1);

    if (var_str.empty()){
        std::cout << "failed to read parameter " << key << std::endl;
        return 1;
    }
    else {
        return atoi(var_str.c_str());
    }
}

int read_params(std::string path, InitData* data){
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    data->Dt = parse_double(content, std::string("Dt"));
    data->N_uav = parse_int(content, std::string("N_uav"));
    data->R = parse_double(content, std::string("R"));
    data->X0 = parse_double(content, std::string("X0"));
    data->Y0 = parse_double(content, std::string("Y0"));
    data->Z0 = parse_double(content, std::string("Z0"));
    data->V0 = parse_double(content, std::string("V0"));
    data->Az = parse_double(content, std::string("Az"));
    data->TimeLim = parse_double(content, std::string("TimeLim"));
    
    return 0;
}
