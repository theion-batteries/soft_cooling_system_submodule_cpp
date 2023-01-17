/**
 * @file ph_cooling_controller.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ph_cooling_controller.h"


ph_cooling_controller::ph_cooling_controller()
{
    std::cout << "creating subsystem wafer holder motion controller " << std::endl;
#ifdef PH_CONFIG
    std::cout << "loading config file: " << PH_CONFIG << std::endl;
    std::ifstream filein(PH_CONFIG);
    for (std::string line; std::getline(filein, line); )
    {
        std::cout << line << std::endl;
    }
    config = YAML::LoadFile(PH_CONFIG);
    _ph_params.distance_to_center = config["distance_to_center"].as<double>();
    _ph_params.number_of_rotation_per_direction = config["number_of_rotation_per_direction"].as<int>();
    _ph_params.reverse_direction = config["reverse_direction"].as<bool>();
    _ph_params.path_to_upload_img = config["path_to_upload_img"].as<std::string>();
    _ph_params.jobid = config["jobid"].as<DWORD>();
    _ph_params.jobtype = config["jobtype"].as<DWORD>();
    _ph_params.res = config["res"].as<DWORD>();
    _ph_params.docwidth = config["docwidth"].as<DWORD>();
    _ph_params.ncopies = config["ncopies"].as<DWORD>();
    _ph_params.docid = config["docid"].as<DWORD>();
    _ph_params.scanning = config["scanning"].as<int>();
#endif 
#ifdef SINK_SENSOR_MOCK
    ph = std::make_shared< sensorMock>();
#else
    ph = std::make_shared< meteorAdapter>();
#endif
#ifdef SINK_AXIS_MOCK
    linearMover = std::make_shared< axisMock>();
#else
    linearMover = std::make_shared< linear_motion>();
#endif
#ifdef SINK_ROT_MOCK
    linearMover = std::make_shared< axisMock>();
#else
    rotaryMover = std::make_shared< rotation_motion>();
#endif
}
/**
 * @brief Destroy the whs controller::whs controller object
 *
 */
ph_cooling_controller::~ph_cooling_controller()
{
}

/**************** Algorithms conntroller ***************/
void ph_cooling_controller::ph_controller_connect()
{
    linearMover->connect();
    rotaryMover->connect();
    ph->connect();
}
void ph_cooling_controller::ph_motion_move_home()
{
    linearMover->move_home();
    rotaryMover->move_home();
}
void ph_cooling_controller::ph_motion_move_to_center(double new_pos)
{
    linearMover->move_down_to(new_pos);
}



/********* helper functions */
bool ph_cooling_controller::get_linear_mover_status()
{
    return linearMover->getStatus();
}
bool ph_cooling_controller::get_rotary_mover_status()
{
    return linearMover->getStatus();
}
bool ph_cooling_controller::get_ph_status()
{
    return ph->getStatus();
}
bool ph_cooling_controller::get_ph_cooling_controller_status()
{
    return phCoolingControllerReady;
}

/*                getter interface              */


double ph_cooling_controller::get_axis_position()
{

    return linearMover->get_position();
}

Iaxis_motion* ph_cooling_controller::get_axis_ptr()
{
    return dynamic_cast<Iaxis_motion*>(linearMover.get());
}
meteorAdapter* ph_cooling_controller::get_ph_ptr()
{
    return dynamic_cast<meteorAdapter*>(ph.get());
}
/**
 * @brief TODO implement direct send cmd to sensor if needed
 *
 * @param cmd
 */
void ph_cooling_controller::sendDirectCmdSensor(std::string& cmd)
{
    // ph->sendDirectCmd(cmd);
}
std::string ph_cooling_controller::sendDirectCmdAxis(std::string cmd)
{
    return  linearMover->sendDirectCmd(cmd);
}

void ph_cooling_controller::reload_config_file()
{

    std::cout << "reloading config file: " << PH_CONFIG << std::endl;
    std::ifstream filein(PH_CONFIG);
    for (std::string line; std::getline(filein, line); )
    {
        std::cout << line << std::endl;
    }
    config = YAML::LoadFile(PH_CONFIG);
    _ph_params.distance_to_center = config["distance_to_center"].as<double>();
    _ph_params.number_of_rotation_per_direction = config["number_of_rotation_per_direction"].as<int>();
    _ph_params.reverse_direction = config["reverse_direction"].as<bool>();
    _ph_params.path_to_upload_img = config["path_to_upload_img"].as<std::string>();
    _ph_params.jobid = config["jobid"].as<DWORD>();
    _ph_params.jobtype = config["jobtype"].as<DWORD>();
    _ph_params.res = config["res"].as<DWORD>();
    _ph_params.docwidth = config["docwidth"].as<DWORD>();
    _ph_params.ncopies = config["ncopies"].as<DWORD>();
    _ph_params.docid = config["docid"].as<DWORD>();
    _ph_params.scanning = config["scanning"].as<int>();

}