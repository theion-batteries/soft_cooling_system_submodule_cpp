/**
 * @file ph_cooling_controller.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "sockpp/socket.h"
#include <iostream>
#include <string>
#include "sockpp/tcp_connector.h"
#include "Windows.h" 
#include <shellapi.h> 
#include <map>
#include <queue>
#include "memory.h"
#include <thread>
#include <future>
#include <atomic>
#include <filesystem>
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "meteorAdapter.h"
#include "ph_xy_motion.h"
#include "ph_trigger.h"
class ph_cooling_controller
{

private:
    bool phCoolingControllerReady = false;
    std::shared_ptr< meteorAdapter> ph;
    std::shared_ptr <Iph_xy_motion> motionMover;
    std::shared_ptr <ph_trigger> phTrigger;
    double current_axis_position;
    YAML::Node config;
    cooling_config_yaml_params _ph_params;
    cooling_config_yaml_params _ph_params_default;
public:
    /******* const/desctr ****/
    ph_cooling_controller();
    ~ph_cooling_controller();
    /******** algorithms controller **********/
    wgm_feedbacks::enum_sub_sys_feedback ph_connect_engine();
    wgm_feedbacks::enum_sub_sys_feedback ph_controller_connect();
    wgm_feedbacks::enum_sub_sys_feedback ph_controller_disconnect();
    wgm_feedbacks::enum_sub_sys_feedback ph_motion_move_home();
    wgm_feedbacks::enum_sub_sys_feedback ph_motion_move_offset();
    wgm_feedbacks::enum_sub_sys_feedback ph_motion_home_all();
    wgm_feedbacks::enum_sub_sys_feedback ph_motion_rotate_home();
    wgm_feedbacks::enum_sub_sys_feedback ph_rotate_to_center(double new_pos);
    wgm_feedbacks::enum_sub_sys_feedback ph_motion_move_to_center(double new_pos);
    wgm_feedbacks::enum_sub_sys_feedback ph_rotate_center();
    wgm_feedbacks::enum_sub_sys_feedback ph_move_center();
    wgm_feedbacks::enum_sub_sys_feedback ph_trigger_print();
    wgm_feedbacks::enum_sub_sys_feedback ph_rotate_and_print();
    
    double get_center_target_distance();
    double get_rotate_to_center_param();
    /********* helper functions */
    bool get_linear_mover_status();
    bool get_rotary_mover_status();
    bool get_trigger_status();
    bool get_ph_status();
    bool get_ph_cooling_controller_status();
    /*     helper getter */
    double get_axis_position();
    double get_rotation_position();
    Iph_xy_motion* get_xy_axis_ptr();
    ph_trigger* get_trigger_ptr();
    meteorAdapter* get_ph_ptr();
    void sendDirectCmdSensor(std::string& cmd);
    std::string sendDirectCmdAxis(std::string cmd);
    void reload_config_file();

};
