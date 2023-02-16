/**
 * @file ph_trigger.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-16
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
#include <thread>
#include <atomic>
#include <queue>
#include <future>
#include "subsystem_feedback.h"
using enum wgm_feedbacks::enum_hw_feedback;
using enum wgm_feedbacks::enum_sub_sys_feedback;
struct ph_trigger_server
{
    std::string ip = "192.168.0.205";
    uint16_t port = 8881;

};


class ph_trigger
{
private:
    ph_trigger_server _trigger_struct;
    sockpp::socket_initializer sockInit;
    sockpp::tcp_connector* trigger_client_sock = nullptr;
    bool triggerReady = false;
    std::map<std::string, std::string> trigger_cmds = {
        {"turn_on","trigger"}, {"turn_off","off"}
    };
    std::string trigger_incoming_data;
    size_t trigger_data_length = 5012;

public:
    ph_trigger(std::string ip, uint16_t port);
    wgm_feedbacks::enum_sub_sys_feedback connect();
    wgm_feedbacks::enum_sub_sys_feedback disconnect();
    wgm_feedbacks::enum_sub_sys_feedback turn_on();
    wgm_feedbacks::enum_sub_sys_feedback turn_off();
    std::string sendDirectCmd(std::string cmd) ;
    std::string waitForResponse();
};



