
#include <iomanip>

#include "../include/uav.hpp"
#include "../include/command.hpp"
#include "../include/Vec3.hpp"


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


Uav::Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az) : position{Vec3(X0,Y0,Z0)}, 
serial_number{N}, min_turn_r{R} {
    double Az_rad = deg_to_rad(Az);
    speed = Vec3(V0 * cos(Az_rad), V0 * sin(Az_rad), 0);
    radial_acc_ability =  (speed.mag() * speed.mag() / min_turn_r);
    exists_target = false;
    file_name = "io_files/UAV" + std::to_string(serial_number) + ".txt";
    out_stream = std::ofstream(file_name);
}

double Uav::get_azimuth(){ // returns values in (-PI, PI]
    return atan2(speed.y, speed.x);
}

void Uav::emit_data(double time){
    out_stream.open(file_name, std::ios_base::app);
    out_stream << std::fixed << std::setprecision(3) << time << " " 
    << std::setprecision(2) << position.x << " " << position.y <<
        " " << rad_to_deg(this->get_azimuth()) << std::endl;
    out_stream.close();
}

void Uav::recieve_command(Command c){
    current_target = Vec3(c.x, c.y, 0);
    exists_target = true;
}

void Uav::update(double dt){
    // check heading in relation to target. if no target just go straight.
    // changin steering state depending on the check.
    // do this by calculating angle alpha between path to target and uav azimuth.
    double epsilon_turn = dt;
    Vec3 acc = Vec3();

    if (exists_target){
        Vec3 current_target_xy = Vec3(current_target.x, current_target.y, 0);
        Vec3 position_xy = Vec3(position.x, position.y, 0);
        Vec3 path = (current_target_xy - position_xy).normalize();
        Vec3 planar_speed = Vec3(speed.x, speed.y, 0);

        double alpha = rad_norm_to_npi_to_pi(this->get_azimuth() - atan2(path.y, path.x));
        if (alpha >= epsilon_turn) acc = (planar_speed.normalize().cross(Vec3(0,0,1))) * radial_acc_ability;
        else if (alpha <= -epsilon_turn) acc = (planar_speed.normalize().cross(Vec3(0,0,-1))) * radial_acc_ability;
        else acc = Vec3(0,0,0);
    }
    else {
        acc = Vec3(0,0,0);
    }

    // update speed depending on turn state.
    speed = speed + acc * dt;

    // update position depending on speed.
    position = position + speed * dt;
}

