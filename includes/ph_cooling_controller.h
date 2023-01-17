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

 //#define MOCK_SENSOR
#ifdef SINK_SENSOR_MOCK
#include "sensorMock.h"
#else
#include "meteorAdapter.h"
#endif
#ifdef SINK_AXIS_MOCK
#include "axisMock.h"
#else
#include "linear_motion.h"
#endif
<<<<<<< HEAD


class ph_cooling_controller
{
public:
     YAML::Node config;
     PrintingParameters ph_configs;
=======
#ifdef SINK_ROT_MOCK
#include "axisMock.h"
#else
#include "rotation_motion.h"
#endif
struct cooling_config_yaml_params
{
    double distance_to_center = 50; // after homing = go to center position where cnt dispenser will vibrate
    int number_of_rotation_per_direction = 10; // frequency for dispensing
    bool reverse_direction = false; // duration of vibration 100 ms
    std::string path_to_upload_img = ""; //optional path for img direct load
    DWORD jobid = 0;					// Default to 0
    DWORD jobtype = JT_PRELOAD;			// Default to Preload path
    DWORD res = RES_HIGH;				// Default to high resolution
    DWORD docwidth = 3508;				// Default to A4 length @ 300dpi
    DWORD ncopies = 1;					// Default to 1 copy of the document
    DWORD docid = 1;					// Default to a document ID of 1
    int scanning = 0;					// Default to not scanning
};

class ph_cooling_controller
{

>>>>>>> 80e4b743bfa5a9812f1fac6948a745714ac40ca0
private:
    bool phCoolingControllerReady = false;
    std::shared_ptr< meteorAdapter> ph;
    std::shared_ptr <Iaxis_motion> linearMover;
    std::shared_ptr <Iaxis_motion> rotaryMover;
    double current_axis_position;
    YAML::Node config;
    cooling_config_yaml_params _ph_params;
public:
    /******* const/desctr ****/
    ph_cooling_controller();
    ~ph_cooling_controller();
    /******** algorithms controller **********/
    void ph_controller_connect();
    void ph_motion_move_home();
    void ph_motion_move_to_center(double new_pos);







    /********* helper functions */
    bool get_linear_mover_status();
    bool get_rotary_mover_status();
    bool get_ph_status();
    bool get_ph_cooling_controller_status();
    /*     helper getter */
    double get_axis_position();
    Iaxis_motion* get_axis_ptr();
    meteorAdapter* get_ph_ptr();
    void sendDirectCmdSensor(std::string& cmd);
    std::string sendDirectCmdAxis(std::string cmd);
    void reload_config_file();

};
