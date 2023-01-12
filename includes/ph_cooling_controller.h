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

struct cooling_config_yaml_params
{
  

};

class ph_cooling_controller
{
public:
     YAML::Node config;
     cooling_config_yaml_params _whs_params;
private:
    bool phCoolingControllerReady = false;
    std::shared_ptr< meteorAdapter> ph;
    std::shared_ptr <Iaxis_motion> linearMover;
    double current_axis_position;
public:
    /******* const/desctr ****/
    ph_cooling_controller();
    ~ph_cooling_controller();
    /******** algorithms controller **********/







    /********* helper functions */
    bool get_mover_status();
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
