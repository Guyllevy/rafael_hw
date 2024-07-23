#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iomanip>

#include "Vec3.hpp"
#include "uav.hpp"
#include "command.hpp"
#include "init_data.hpp"

bool compareByTime(const Command& a, const Command& b) {
    return a.time < b.time;
}

int main(int argc, char* argv[]){

    bool centered = false;
    if (argc > 1 && std::string(argv[1]) == "--centered") centered = true;

    InitData* data = new InitData;
    std::vector<Command> commands;
    
    // read initialization inputs
    read_params("io_files/SimParams.ini", data);
    
    // read commands
    read_commands("io_files/SimCmds.txt", commands);

    // sort commands by time
    std::sort(commands.begin(), commands.end(), compareByTime);

    // initialize vehicles
    std::vector<Uav*> vehicles;
    Uav* v;
    for (int i = 0 ; i < data->N_uav ; i++){
        v = new Uav(i, data->R, data->X0, data->Y0, data->Z0, data->V0, data->Az, centered);
        vehicles.push_back(v);
    }


    double time = 0.0;
    int command_counter = 0;
    Command pending_command;
    bool command_pending = false;
    if (commands.size() > 0){
        pending_command = commands[0];
        command_pending = true;
    }

    while (time < data->TimeLim){     // run loop, outputing telemetry
        
        // send all commands which are due time.
        while (command_pending && time >= pending_command.time){ 
            vehicles[pending_command.uav_sn]->recieve_command(pending_command);
            command_counter++;
            if (command_counter < commands.size()){
                pending_command = commands[command_counter];
            }
            else command_pending = false;
        }

        // update vehicles state and emit simulation state
        for (auto v : vehicles){
            v->emit_data(time);
            v->update(data->Dt);
        } 

        // keeping time
        time += data->Dt;

    }

    return 0;
}