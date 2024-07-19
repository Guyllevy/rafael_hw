#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iomanip>

#include "Vec3.hpp"

const double PI = 3.14159265359;

enum steering_state {
    STEER_RIGHT,
    STEER_LEFT,
    STEER_AHEAD
};

double deg_to_rad(double angle){
    return (angle / 180) * PI;
}
double rad_to_deg(double angle){
    return (angle / PI) * 180;
}

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
};

void show_init_data(InitData* data){
    std::cout << "N_uav: " << data->N_uav << std::endl;
    std::cout << "Dt: " << data->Dt << std::endl;
    std::cout << "R: " << data->R << std::endl;
    std::cout << "X0: " << data->X0 << std::endl;
    std::cout << "Y0: " << data->Y0 << std::endl;
    std::cout << "Z0: " << data->Z0 << std::endl;
    std::cout << "V0: " << data->V0 << std::endl;
    std::cout << "Az: " << data->Az << std::endl;
    std::cout << "TimeLim: " << data->TimeLim << std::endl;
}

struct Command{

    double time;
    int uav_sn;
    double x;
    double y;

    void show(){
        std::cout << "By Value - Serial Number: " << this->uav_sn 
                  << ", Time: " << this->time 
                  << ", X: " << this->x 
                  << ", Y: " << this->y << std::endl;
    }
};

bool compareByTime(const Command& a, const Command& b) {
    return a.time < b.time;
}

class Uav{
    private:
    int serial_number;
    double min_turn_r;
    Vec3 position;
    Vec3 speed; // speed defines azimut.
    Vec3 acc_ability_right; // the accelaration acting on a uav turning right. (minus this will be left)
    bool exists_target;
    Vec3 current_target;
    steering_state state;

    public:
    Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az) : position{Vec3(X0,Y0,Z0)}, 
    serial_number{N}, min_turn_r{R} {
        double Az_rad = deg_to_rad(Az);
        speed = Vec3(V0 * cos(Az_rad), V0 * sin(Az_rad), 0);
        acc_ability_right = speed.normalize().cross(Vec3(0,0,1)) * (speed.mag() * speed.mag() / min_turn_r);
        exists_target = false;
        state = STEER_AHEAD;
    }

    double get_azimuth(){
        return Vec3(1,0,0).angle(this->speed);
    }

    void emit_pos(){
        std::cout << serial_number << " " << position.x << " " << position.y << " " << position.z << std::endl;
    }

    void get_command(Command c){
        current_target = Vec3(c.x, c.y, 0);
    }

    void update(double dt){
        // check heading in relation to target. if no target just go straight.
        // changin steering state depending on the check.
        // do this by calculating angle alpha between path to target and uav azimuth.
        double epsilon_turn = 0.05;
        if (exists_target){
            Vec3 current_target_xy = Vec3(current_target.x, current_target.y, 0);
            Vec3 position_xy = Vec3(position.x, position.y, 0);
            Vec3 path = (current_target_xy - position_xy).normalize();

            double alpha = this->get_azimuth() - Vec3(1,0,0).angle(path); // [-pi, pi]

            if (alpha >= epsilon_turn) state = STEER_LEFT;
            else if (alpha <= -epsilon_turn) state = STEER_RIGHT;
            else state = STEER_AHEAD;
        }
        else {
            state = STEER_AHEAD;
        }

        Vec3 acc = Vec3();
        if (state == STEER_RIGHT) acc = acc_ability_right;
        else if (state == STEER_LEFT) acc = Vec3(0,0,0) - acc_ability_right;
        else acc = Vec3(0,0,0);

        // update speed depending on turn state.
        speed = speed + acc * dt;

        // update position depending on speed.
        position = position + speed * dt;

    }
};

double parse_double(std::string content, std::string key) {
    std::regex re(key + R"(\s*=\s*\d+(\.\d+)?)");
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

int main(){
    // define output file
    std::ofstream out("output.txt");
    if (!out) {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }
    std::streambuf* cout_buf = std::cout.rdbuf(out.rdbuf());

    std::cout << "AHEAD " << STEER_AHEAD << std::endl;
    std::cout << "LEFT " << STEER_LEFT << std::endl;
    std::cout << "RIGHT " << STEER_RIGHT << std::endl;

    InitData* data = new InitData;
    std::vector<Command> commands;
    
    // read initialization inputs
    read_params("SimParams.ini", data);
    show_init_data(data);
    
    // read commands
    read_commands("SimCmds.txt", commands);

    // sort commands by time
    std::sort(commands.begin(), commands.end(), compareByTime);
    for (auto c : commands){
        c.show();
    }

    // initialize vehicles
    std::vector<Uav*> vehicles;
    Uav* v;
    for (int i = 0 ; i < data->N_uav ; i++){
        v = new Uav(i, data->R, data->X0, data->Y0, data->Z0, data->V0, data->Az);
        vehicles.push_back(v);
    }


    double time = 0.0;
    int command_counter = 0;
    Command pending_command = commands[0];
    while (time < data->TimeLim){     // run loop, outputing telemetry
        
        // send all commands which are due time.
        while (time >= pending_command.time){ 
            vehicles[pending_command.uav_sn]->get_command(pending_command);
            command_counter++;
            pending_command = commands[command_counter];
        }

        // update vehicles state and emit simulation state
        for (auto v : vehicles){
            std::cout << std::fixed << std::setprecision(3) << time << std::setprecision(2) << " ";
            v->emit_pos();
            v->update(data->Dt);
        } 

        // keeping time
        time += data->Dt;
    }

    // Restore standard output to console
    std::cout.rdbuf(cout_buf);
    out.close();

    return 0;
}