#ifndef UAV_HPP
#define UAV_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "command.hpp"
#include "Vec3.hpp"


class Uav{
    
private:

    int serial_number;
    double min_turn_r;
    Vec3 position;
    Vec3 speed; // speed defines azimut.
    double radial_acc_ability; // the accelaration acting on a uav turning right. (minus this will be left)
    bool exists_target;
    Vec3 current_target;
    std::string file_name;
    std::ofstream out_stream;

    int behavior;

    // for the second behavior
    bool waiting;
    bool changed_target;

public:

    Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az, int behave = 1);
    double get_azimuth(); // returns values in (-PI, PI]
    void emit_data(double time);
    void recieve_command(Command c);
    void update(double dt);

};

#endif