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
    // static members
    const int serial_number;
    const double min_turn_r;

    std::string file_name;
    std::ofstream out_stream;

    // dynamic members
    Vec3 position;
    Vec3 speed; // speed defines azimut.

    bool exists_target;
    Vec3 current_target;
    bool centered; // spining around the target instead of tangentialy. default is false.
    Vec3 space_left; // used to approach target to circulate around it.

    bool waiting;
    bool changed_target;


public:

    Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az, bool centered = false);
    ~Uav();
    double get_azimuth(); // returns values in (-PI, PI]
    void emit_data(double time);
    void recieve_command(Command c);
    void update(double dt);
    Vec3 get_speed();

};

#endif