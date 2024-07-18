#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cmath>

const double PI = 3.14159265359;

double deg_to_rad(double angle){
    return (angle / 180) * PI;
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

struct Command{

    double time;
    int serial_number;
    double x;
    double y;

    void show(){
        std::cout << "By Value - Serial Number: " << this->serial_number 
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
    double position[3];
    double min_turn_r;
    double speed[3]; // speed defines azimut.
    double speed_mag;
    double azimuth;
    bool exists_goal;
    double current_goal[3];

    public:
    Uav(int N, double R, double X0, double Y0, double Z0, double V0, double Az){
        this->serial_number = N;
        this->min_turn_r = R;
        this->position = {X0, Y0, Z0};
        this->azimuth = Az;
        this->speed_mag = V0;
        double Az_rad = deg_to_rad(Az);
        this->speed = {V0 * cos(Az_rad), V0 * sin(Az_rad) , 0};
        this->exists_goal = false;
    }
    void print_data();
    void get_command(double new_goal[3]);
    void update();
};

double parse_double(std::string content, std::string key) {
    std::regex re(key + R"(\s*=\s*\d+(\.\d+)?)");
    std::smatch match;
    int found = std::regex_search(content, match, re);
    std::string var_str = match[0].str().substr(match[0].str().find("=") + 1);
    std::cout << "var str " << var_str << std::endl;
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
    std::cout << "var str " << var_str << std::endl;
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
            iss >> c.time >> c.serial_number >> c.x >> c.y;
            commands.push_back(c);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }

    return 0;

}

int main(){
    InitData* data = new InitData;
    std::vector<Command> commands;
    
    // read initialization inputs
    read_params("SimParams.ini", data);
    
    // read commands
    read_commands("SimCmds.txt", commands);

    // sort commands by time
    std::sort(commands.begin(), commands.end(), compareByTime);

    // initialize vehicles
    std::vector<Uav*> vehicles;
    std::Uav* v;
    for (int i = 0 ; i < data.N_uav ; i++){
        v = new Uav(i, data->R, data->X0, data->Y0, data->Z0, data->V0, data->Az);
    }


    double time = 0.0;
    int command_counter = 0;
    Command pending_command = commands[0];
    while (time < data->TimeLim){     // run loop, outputing telemetry
        
        time += data->Dt;
    }

    return 0;
}