#ifndef INIT_DATA_HPP
#define INIT_DATA_HPP

#include <regex>
#include <string>
#include <fstream>
#include <sstream>


struct InitData{
    int N_uav;
    double Dt;
    double R;
    double X0;
    double Y0;
    double Z0;
    double V0;
    double Az;
    double TimeLim;

    void show();
};

double parse_double(std::string content, std::string key);

int parse_int(std::string content, std::string key);

int read_params(std::string path, InitData* data);

#endif