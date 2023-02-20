/**
 * @file Iph_xy_motion.h
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

#include "subsystem_feedback.h"
using enum wgm_feedbacks::enum_hw_feedback;
using enum wgm_feedbacks::enum_sub_sys_feedback;
class Iph_xy_motion
{
private:
    /* data */
public:
    // generic
    virtual ~Iph_xy_motion(){};
    virtual wgm_feedbacks::enum_sub_sys_feedback connect() = 0;
    virtual wgm_feedbacks::enum_sub_sys_feedback disconnect() = 0;
    virtual std::string sendDirectCmd(std::string cmd)=0;
    virtual std::string waitForResponse() =0;
    virtual bool getStatus() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback unlock() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback pause() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback resume() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback home_all() =0;

    // linear
    virtual wgm_feedbacks::enum_sub_sys_feedback move_home() = 0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_to(double new_position) = 0;
    virtual double get_rotation_position() = 0;
    virtual double get_rotation_speed() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback set_Xspeed(double_t new_val) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_up_to(double_t new_pos) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_down_to(double_t new_pos) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_up_by(double_t steps) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_down_by(double_t steps) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback move_center() = 0;
    // roatation
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_home() = 0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_to(double new_position) = 0;
    virtual double get_linear_position() = 0;
    virtual double get_linear_speed() =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback set_Yspeed(double_t new_val) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_up_to(double_t new_pos) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_down_to(double_t new_pos) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_up_by(double_t steps) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_down_by(double_t steps) =0;
    virtual wgm_feedbacks::enum_sub_sys_feedback rotate_center() = 0;




};



