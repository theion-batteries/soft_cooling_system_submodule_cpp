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
    std::cout << "creating subsystem cooling ph controller " << "\n";
#ifdef PH_CONFIG
    std::cout << "loading config file: " << PH_CONFIG << "\n";
    std::ifstream filein(PH_CONFIG);
    for (std::string line; std::getline(filein, line);)
    {
        std::cout << line << "\n";
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
    _ph_params.timeout = config["timeout"].as<uint16_t>();

#endif

    ph = std::make_shared<meteorAdapter>(_ph_params);

    motionMover = std::make_shared<ph_xy_motion>(_ph_params.ph_motion_server_ip, _ph_params.ph_motion_server_port, _ph_params.timeout);

    phTrigger = std::make_shared<ph_trigger>(_ph_params.ph_trigger_server_ip, _ph_params.ph_trigger_server_port, _ph_params.timeout);
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
    if (ph->connect() == hw_error)
        return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_controller_connect()
{
    if (motionMover->connect() == sub_error || phTrigger->connect() == sub_error /*|| ph->connect() == hw_error*/)
        return sub_error;
    phCoolingControllerReady = true;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_controller_disconnect()
{
    if (motionMover->disconnect() == sub_error || phTrigger->disconnect() == sub_error || ph->disconnect() == hw_error)
        return sub_error;
    phCoolingControllerReady = false;
    return sub_success;
}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_home_all()
{
    if (motionMover->home_all() == sub_error)
        return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_home()
{
    if (motionMover->move_home() == sub_error)
        return sub_error;
    return sub_success;
}

/*wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_offset()
{
    double offset_pos = 50; // offset position in mm
    if (motionMover->move_down_to(offset_pos) == sub_error)
        return sub_error;
    return sub_success;
}
*/
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_offset(const double offset)
{
    if (motionMover->move_down_to(offset) == sub_error)
        return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_rotate_home()
{
    if (motionMover->rotate_home() == sub_error)
        return sub_error;
    return sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_motion_move_to_center(const double new_pos)
{
    return motionMover->move_down_to(new_pos);
}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_to_center(const double degree)
{
    return motionMover->rotate_to(degree);
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_to(const double degree)
{
    return motionMover->rotate_to(degree);
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_move_center()
{
    return motionMover->move_down_to(_ph_params.distance_to_center);
}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_center()
{
    return motionMover->rotate_to(_ph_params.ph_rotate_to_center);
}
wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_trigger_print()
{
    return phTrigger->turn_on();
}

wgm_feedbacks::enum_sub_sys_feedback ph_cooling_controller::ph_rotate_and_print()
{

    /* TODO
    // rotate thread
    // print thread
    // Algoritm:
    1. start rotating freely:
    2. after 1 rev: start printing
    3. after 5 rev: stop printing and rotation
    */

    const double one_rotation = 360; // degrees to finish to one rotation
    const double complete_roation = _ph_params.phead_rotations * one_rotation;
    const double home_rotation = -90;
    ph_xy_motion::setModeBlocking(true);
    auto result = motionMover->rotate_to(one_rotation); // complete one rotation
    if (result == sub_error)
     {
        std::cout<<" Failed to rotate. exiting . \n";
        return sub_error;
     }

    result = phTrigger->turn_on();
    if (result == sub_error)
     {
        std::cout<<" Failed to trigger. exiting . \n";
        return sub_error;
     }

    ph_xy_motion::setModeBlocking(true);
    result = motionMover->rotate_to(complete_roation);
    if (result == sub_error)
     {
        std::cout<<" Failed to rotate complete number of turns. exiting. \n";
        return sub_error;
     }

/*    ph_xy_motion::setModeBlocking(true);
    result = motionMover->rotate_to(home_rotation);
    if (result == sub_error)
     {
        std::cout<<" Failed to rotate to home position. \n";
        return sub_error;
     } */

    return sub_success;
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
    return motionMover->getStatus();
}
bool ph_cooling_controller::get_rotary_mover_status()
{
    return motionMover->getStatus();
}
bool ph_cooling_controller::get_trigger_status()
{
    return phTrigger->getStatus();
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

    return motionMover->get_linear_position();
}

double ph_cooling_controller::get_rotation_position()
{

    return motionMover->get_rotation_position();
}

double ph_cooling_controller::get_axis_velocity()
{

    return motionMover->get_linear_speed();
}

double ph_cooling_controller::get_rotation_velocity()
{

    return motionMover->get_rotation_speed();
}

Iph_xy_motion *ph_cooling_controller::get_xy_axis_ptr()
{
    return dynamic_cast<Iph_xy_motion *>(motionMover.get());
}
ph_trigger *ph_cooling_controller::get_trigger_ptr()
{
    return dynamic_cast<ph_trigger *>(phTrigger.get());
}
meteorAdapter *ph_cooling_controller::get_ph_ptr()
{
    return dynamic_cast<meteorAdapter *>(ph.get());
}
/**
 * @brief TODO implement direct send cmd to sensor if needed
 *
 * @param cmd
 */
void ph_cooling_controller::sendDirectCmdSensor(std::string &cmd)
{
    // ph->sendDirectCmd(cmd);
}

std::string ph_cooling_controller::sendDirectCmdAxis(std::string cmd)
{
    return motionMover->sendDirectCmd(cmd);
}

void ph_cooling_controller::reload_config_file()
{

    std::cout << "reloading config file: " << PH_CONFIG << "\n";
    std::ifstream filein(PH_CONFIG);
    for (std::string line; std::getline(filein, line);)
    {
        std::cout << line << "\n";
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
    _ph_params.timeout = config["timeout"].as<uint16_t>();
}