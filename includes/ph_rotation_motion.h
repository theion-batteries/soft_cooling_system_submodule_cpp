/**
 * @file Iaxis_motion.h
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
#include "Windows.h" 
#include <shellapi.h> 
#include <map>
#include <thread>
#include <atomic>
#include <queue>
#include <future>
#include "Iph_rotation.h"

struct ph_rotation_motion_server
{
    std::string ip = "192.168.0.205";
    uint16_t port = 8882;
    double phead_intermediate_stop = 50;
    double phead_start_angle = 90;
    double phead_max_rot_speed = 100;
    double phead_max_rotations = 50;
    double phead_rotations = 5;

};


class ph_rotation_motion: public Iph_rotation
{
private:
    ph_rotation_motion_server _rotation_motion_struct;
    sockpp::socket_initializer sockInit;
    sockpp::tcp_connector* _client = nullptr;
    bool axisReady = false;
protected:
    std::map<std::string, std::string> axis_cmds = {
        {"unlock","$Y"}, {"get_position","?"}, {"rotate","Y"},
        {"get_setting","$$"}, {"set_speed","$110="}, {"home","$H"},
        {"pause","!"}, {"resume","~"}
    };
    std::deque<double> axis_last_position; // FIFO last 10 values
    std::string incoming_data;
    size_t axis_data_length = 5012;
public:
    ph_rotation_motion();
    ph_rotation_motion(std::string ip, uint16_t port);
    virtual ~ph_rotation_motion();
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_home() override;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_to(double new_position) override;
    virtual wgm_feedbacks::enum_sub_sys_feedback connect()override;
    virtual wgm_feedbacks::enum_sub_sys_feedback disconnect() override;
    double get_position() override;
    double get_speed() override;
    wgm_feedbacks::enum_sub_sys_feedback set_speed(double_t new_val) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_up_to(double_t new_pos) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_down_to(double_t new_pos) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_up_by(double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback rotate_down_by(double_t steps) override;
    wgm_feedbacks::enum_sub_sys_feedback set_center_position(double new_target) ;
    virtual bool getStatus() override;
    virtual std::string sendDirectCmd(std::string cmd) override;
    std::string waitForResponse();
    wgm_feedbacks::enum_sub_sys_feedback rotate_center();
    wgm_feedbacks::enum_sub_sys_feedback unlock();
};

















