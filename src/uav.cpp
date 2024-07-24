
#include <iomanip>
#include <cmath>

#include "uav.hpp"
#include "command.hpp"
#include "Vec3.hpp"

#include <cstring>
#include <cerrno>


const double PI = 3.14159265359;

double deg_to_rad(double angle){
    return (angle / 180) * PI;
}
double rad_to_deg(double angle){
    return (angle / PI) * 180;
}
double rad_norm_to_npi_to_pi(double angle){
    while (angle > PI) angle -= 2*PI;
    while (angle <= -PI) angle += 2*PI;
    return angle;
}


Uav::Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az, bool centered)
 : serial_number{N}, min_turn_r{R}, position{Vec3(X0,Y0,Z0)}, centered{centered} {

    double Az_rad = deg_to_rad(Az);
    speed = Vec3(V0 * cos(Az_rad), V0 * sin(Az_rad), 0);
    space_left = Vec3(0,0,0);
    
    exists_target = false;
    waiting = false;
    changed_target = false;

    file_name = "io_files/UAV" + std::to_string(serial_number) + ".txt";
}

Uav::~Uav(){
    out_stream.open(file_name, std::ofstream::out | std::ofstream::app);
    if (!out_stream.is_open()) {
        std::cerr << "Error opening file: " << file_name << std::endl;
    }
    out_stream << oss.str();
    if (out_stream.is_open()) {
        out_stream.close();
    }
}

double Uav::get_azimuth(){ // returns values in (-PI, PI]
    return atan2(speed.y, speed.x);
}

void Uav::emit_data(double time){
    oss << std::fixed << std::setprecision(3) << time << " "
               << std::setprecision(2) << position.x << " " << position.y << " "
               << rad_to_deg(this->get_azimuth()) << std::endl;
}

void Uav::recieve_command(Command c){
    current_target = Vec3(c.x, c.y, 0);
    exists_target = true;
    changed_target = true;
}

void Uav::update(double dt){
    
    // check heading in relation to target. if no target just go straight.
    // changin steering state depending on the check.
    // do this by calculating angle alpha between path to target and uav azimuth.

    double omega = speed.mag()/min_turn_r; // rads/sec
    double epsilon_turn = omega * dt; // in dt seconds the uav wont turn more than w * dt radians
    Vec3 position_xy = Vec3(position.x, position.y, 0);
    Vec3 planar_speed = Vec3(speed.x, speed.y, 0);

    if (exists_target){
        Vec3 target_xy = Vec3(current_target.x, current_target.y, 0);
        Vec3 path = target_xy + space_left - position_xy;
        
        if (changed_target) {
            if (centered) space_left = Vec3(path.normalize()).cross(Vec3(0,0,-1)) * min_turn_r;
            waiting = false;
            changed_target = false;
        }

        double alpha = rad_norm_to_npi_to_pi(this->get_azimuth() - atan2(path.y, path.x));
        if (waiting) speed = speed.rotateZ(-omega*dt); // right
        else if (alpha >= epsilon_turn) speed = speed.rotateZ(-omega*dt); // right
        else if (alpha <= -epsilon_turn) speed = speed.rotateZ(omega*dt); // left

        if (path.mag() < min_turn_r/10){
            waiting = true;
        }
    }

    // update position depending on speed.
    position = position + speed * dt;
}

Vec3 Uav::get_speed(){
    return speed;
}

