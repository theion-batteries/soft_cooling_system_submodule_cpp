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
using enum wgm_feedbacks::enum_hw_feedback;
using enum wgm_feedbacks::enum_sub_sys_feedback;


ph_cooling_controller::ph_cooling_controller()
{
    std::cout << "creating subsystem cooling ph controller " << std::endl;
#ifdef PH_CONFIG
    std::cout << "loading config file: " << PH_CONFIG << std::endl;
    std::ifstream filein(PH_CONFIG);
    for (std::string line; std::getline(filein, line); )
    {
        std::cout << line << std::endl;
    }
    config = YAML::LoadFile(PH_CONFIG);
    _ph_params.ph_rotate_to_center = config["ph_rotate_to_center"].as<double>();
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
    // ips /port
    _ph_params.ph_motion_server_ip = config["ph_motion_server_ip"].as<std::string>();
    _ph_params.ph_motion_server_port = config["ph_motion_server_port"].as<uint16_t>();
    _ph_params.ph_rotation_server_ip = config["ph_rotation_server_ip"].as<std::string>();
    _ph_params.ph_rotation_server_port = config["ph_rotation_server_port"].as<uint16_t>();
    _ph_params.ph_trigger_server_ip = config["ph_trigger_server_ip"].as<std::string>();
    _ph_params.ph_trigger_server_port = config["ph_trigger_server_port"].as<uint16_t>();


    // motion /rotation
    _ph_params.phead_travel = config["phead_travel"].as<double>();
    _ph_params.phead_max_travel = config["phead_max_travel"].as<double>();
    _ph_params.phead_rotations = config["phead_rotations"].as<double>();
    _ph_params.phead_max_rotations = config["phead_max_rotations"].as<double>();
    _ph_params.phead_max_linear_speed = config["phead_max_linear_speed"].as<double>();
    _ph_params.phead_max_rot_speed = config["phead_max_rot_speed"].as<double>();
    _ph_params.phead_intermediate_stop = config["phead_intermediate_stop"].as<double>();
    _ph_params.phead_start_angle = config["phead_start_angle"].as<double>();

#endif 
#ifdef SINK_PH_MOCK
    ph = std::make_shared< sensorMock>();
#else
    ph = std::make_shared< meteorAdapter>(_ph_params);
#endif
#ifdef SINK_AXIS_MOCK
    linearMover = std::make_shared< axisMock>();
#else
    linearMover = std::make_shared< ph_linear_motion>(_ph_params.ph_motion_server_ip ,_ph_params.ph_motion_server_port);
#endif
#ifdef SINK_ROT_MOCK
    linearMover = std::make_shared< axisMock>();
#else
    rotaryMover = std::make_shared< ph_rotation_motion>(_ph_params.ph_rotation_server_ip,_ph_params.ph_rotation_server_port);
#endif
#ifdef SINK_ROT_MOCK
    linearMover = std::make_shared< axisMock>();
#else
    phTrigger = std::make_shared< ph_trigger>(_ph_params.ph_trigger_server_ip,_ph_params.ph_trigger_server_port);
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

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_connect_engine()
{
    if (ph->connect() == hw_error) return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_controller_connect()
{
    if (linearMover->connect() == sub_error || rotaryMover-> connect() == sub_error || phTrigger-> connect() == sub_error || ph_connect_engine() == sub_error) return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_home()
{
    if (linearMover->move_home() == sub_error ||rotaryMover->rotate_home() ==sub_error) return sub_error;
    return sub_success;
}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_to_center(double new_pos)
{
return    linearMover->move_down_to(new_pos);

}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_to_center(double new_pos)
{
return    rotaryMover->rotate_to(new_pos);

}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_move_center()
{
return    linearMover->move_down_to(_ph_params.distance_to_center);

}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_center()
{
return    rotaryMover->rotate_to(_ph_params.ph_rotate_to_center);

}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_and_print()
{

    /*
    // rotate thread 
    // print thread
    // Algoritm:
    1. start rotating freely:
    2. after 1 rev: start printing
    3. after 5 rev: stop printing and rotation
    */
    phTrigger->turn_on();
    return sub_error;
}


double ph_cooling_controller::get_center_target_distance()
{
    return _ph_params.distance_to_center;
}
double ph_cooling_controller::get_rotate_to_center_param()
{
    return _ph_params.ph_rotate_to_center;
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

Iph_axis_motion* ph_cooling_controller::get_axis_ptr()
{
    return dynamic_cast<Iph_axis_motion*>(linearMover.get());
}
Iph_axis_motion* ph_cooling_controller::get_rotary_axis_ptr()
{
    return dynamic_cast<Iph_axis_motion*>(rotaryMover.get());
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
    _ph_params.ph_rotate_to_center = config["ph_rotate_to_center"].as<double>();
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
    // ips /port
    _ph_params.ph_motion_server_ip = config["ph_motion_server_ip"].as<std::string>();
    _ph_params.ph_motion_server_port = config["ph_motion_server_port"].as<uint16_t>();
    _ph_params.ph_rotation_server_ip = config["ph_rotation_server_ip"].as<std::string>();
    _ph_params.ph_rotation_server_port = config["ph_rotation_server_port"].as<uint16_t>();
    _ph_params.ph_trigger_server_ip = config["ph_trigger_server_ip"].as<std::string>();
    _ph_params.ph_trigger_server_port = config["ph_trigger_server_port"].as<uint16_t>();
 
    // motion /rotation
    _ph_params.phead_travel = config["phead_travel"].as<double>();
    _ph_params.phead_max_travel = config["phead_max_travel"].as<double>();
    _ph_params.phead_rotations = config["phead_rotations"].as<double>();
    _ph_params.phead_max_rotations = config["phead_max_rotations"].as<double>();
    _ph_params.phead_max_linear_speed = config["phead_max_linear_speed"].as<double>();
    _ph_params.phead_max_rot_speed = config["phead_max_rot_speed"].as<double>();
    _ph_params.phead_intermediate_stop = config["phead_intermediate_stop"].as<double>();
    _ph_params.phead_start_angle = config["phead_start_angle"].as<double>();
}