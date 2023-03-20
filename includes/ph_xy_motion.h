/**
 * @file ph_xy_motion.h
 * @author sami dhiab
 * @version 0.1
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "sockpp/socket.h"
#include <iostream>
#include <string>
#include "sockpp/tcp_connector.h"
#include <iostream>
#include "Iph_xy_motion.h"
#include "subsystem_feedback.h"
#include <map>

using enum wgm_feedbacks::enum_hw_feedback;
using enum wgm_feedbacks::enum_sub_sys_feedback;



struct ph_motion_server
{
    std::string ip = "192.168.0.205";
    uint16_t port = 8882;
    double phead_intermediate_stop = 50;
    double phead_start_angle = 90;
    double phead_max_rot_speed = 100;
    double phead_max_rotations = 50;
    double phead_rotations = 5;
    double max_travel = 150;            //                    # used # how far the printhead will move, to be adjusted each try
    double phead_max_travel = 220;             //                  # used # DO NOT CHANGE IF YOU DONT KNOW WHAT YOU#RE DOING
    uint16_t timeout = 10;

};

class ph_xy_motion: public Iph_xy_motion
{
private:
    ph_motion_server _motion_struct;
    sockpp::socket_initializer sockInit;
    sockpp::tcp_connector* _client = nullptr;
    bool axisReady = false;
    const double turn_length= 10.00;  // turn length of Print head in mm
    std::vector<std::pair<double,double>> positionXY;
    std::vector<std::pair<double,double>> velocityXY;
    double convert_degree_to_mm(const double degree);

protected:
    std::map<std::string, std::string> axis_cmds = {
        {"unlock","$X"}, {"get_position","?"}, {"rotate","Y"},{"move","X"},
        {"get_setting","$$"}, {"set_Xspeed","$110="}, {"homeX","$HX"},{"homeY","$HY"},
        {"set_Yspeed","$111="},  {"pause","!"}, {"resume","~"},{"homeAll","$H"}
    };
    std::string incoming_data;
    size_t axis_data_length = 5012;
    static inline bool blocking = false;

public:
    //generic
    ph_xy_motion(/* args */);
    ph_xy_motion(std::string ip, uint16_t port, const uint16_t timeout);

    virtual ~ph_xy_motion();
    wgm_feedbacks::enum_sub_sys_feedback connect() override;
    wgm_feedbacks::enum_sub_sys_feedback disconnect() override;
    std::string sendDirectCmd(std::string cmd) override;
    std::string waitForResponse() override;
    bool getStatus() override;
    wgm_feedbacks::enum_sub_sys_feedback unlock() override;
    wgm_feedbacks::enum_sub_sys_feedback pause() override;
    wgm_feedbacks::enum_sub_sys_feedback resume() override;
    wgm_feedbacks::enum_sub_sys_feedback home_all() override;
    // linear
    wgm_feedbacks::enum_sub_sys_feedback move_home() override;
    wgm_feedbacks::enum_sub_sys_feedback move_to(const double new_position) override;
    double get_rotation_position() override;
    double get_rotation_speed() override;
    wgm_feedbacks::enum_sub_sys_feedback set_Xspeed(const double_t new_val) override;
    wgm_feedbacks::enum_sub_sys_feedback move_up_to(const double_t new_pos) override;
    wgm_feedbacks::enum_sub_sys_feedback move_down_to(const double_t new_pos) override;
    wgm_feedbacks::enum_sub_sys_feedback move_up_by(const double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback move_down_by(const double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback move_center() override;
    wgm_feedbacks::enum_sub_sys_feedback set_linear_Center_position(const double new_target);

    // roatation
    wgm_feedbacks::enum_sub_sys_feedback rotate_home() override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_to(const double new_position) override;
    double get_linear_position() override;
    double get_linear_speed() override;
    wgm_feedbacks::enum_sub_sys_feedback set_Yspeed(const double_t new_val) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_up_to(const double_t degree) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_down_to(const double_t degree) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_up_by(const double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_down_by(const double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_center() override;
    wgm_feedbacks::enum_sub_sys_feedback set_rotation_Center_position(const double new_target);
    virtual std::string get_settings() override;
    virtual std::pair<double, double> get_xy_position()override;
    virtual std::pair<double, double> get_xy_velocity()override;

    static void setModeBlocking(bool setblockingMode);
};



