/**
 * @file Iaxis_motion.h
 * @author sami dhiab
 * @version 0.1
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ph_linear_motion.h"
#include <thread>
#include <mutex>
#include <string.h>

ph_linear_motion::ph_linear_motion(std::string ip, uint16_t port)
{
    std::cout << "creating linear axis client" << "\n";
    _motion_axis_struct.ip = ip;
    _motion_axis_struct.port = port;

}
ph_linear_motion::~ph_linear_motion()
{
    if (_client != nullptr) delete _client;

}
std::string ph_linear_motion::sendDirectCmd(std::string cmd)
{
    if (_client == nullptr) return "not connected";
    std::cout << "sending linear axis command " << cmd << "\n";
    cmd = cmd + "\r\n";

    if (_client->write(cmd) != ssize_t(std::string(cmd).length())) {
        std::cout << "Error writing to the TCP stream: "
            << _client->last_error_str() << "\n";
    }
    return waitForResponse();
}
std::string ph_linear_motion::waitForResponse()
{
 //    // std::cout << "awaiting server response" << "\n";
    auto start = std::chrono::steady_clock::now();
    while (_client->is_connected())
    {

        char Strholder[5012];
        
            ssize_t n = _client->read_n(&Strholder, sizeof(Strholder));
        if (n > 0)
        {
            std::cout << "n bytes received: " << n << "\n";
            incoming_data = Strholder;
            incoming_data.resize(n);
            std::cout << "server replied : " << incoming_data << "\n";
            break;
        }
        else
        {
            // std::cout << "no server response, retry " << n << "\n";
            incoming_data = "NA";
            long long timeout = 10;
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
            if(duration >= timeout)
            {
            std::cout << "no response within a timeout of "<<duration<< " seconds, " <<"aborting.."<< "\n";
            break;
            } 
            continue;
        }

    }
    return incoming_data;
}
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::set_center_position(double new_target)
{
    _motion_axis_struct.max_travel = new_target;
    return sub_success;
}


bool ph_linear_motion::getStatus()
{
    return axisReady;
}


/**
 * @brief TODO: change the function call
 *
 * @param new_position
 */
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_to(int new_position)
{
    if (new_position < 0) 
    {
        if(move_down_to(abs(new_position)) == sub_success) return sub_success;
        return sub_error;
    } 
    else
    {
        if(move_down_to(abs(new_position)) == sub_success) return sub_success;
                return sub_error;

    }
    
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::connect()
{
    std::cout << "connecting controller to axis server" << "\n";
    std::cout << "axis server ip:  " << _motion_axis_struct.ip << "\n";
    _client = new sockpp::tcp_connector({ _motion_axis_struct.ip, _motion_axis_struct.port });
    // Implicitly creates an inet_address from {host,port}
    // and then tries the connection.
    if (!_client->is_connected()) {
        std::cerr << "Error connecting to axis server at "
            << sockpp::inet_address(_motion_axis_struct.ip, _motion_axis_struct.port)
            << " -> " << _client->last_error_str();
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    std::cout << "Created a connection from " << _client->address() << "\n";
    std::cout << "Created a connection to " << _client->peer_address() << "\n";
    // Set a timeout for the responses
    if (!_client->read_timeout(std::chrono::seconds(5))) {
        std::cerr << "Error setting timeout on TCP stream: "
            << _client->last_error_str() << "\n";
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    axisReady = true;
    return wgm_feedbacks::enum_sub_sys_feedback::sub_success;

}

wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::disconnect()
{
    axisReady = false;
    if (_client->close()) return sub_success;
    return sub_error;
}


/**
 * @brief
 *
 * @return double
 */
double ph_linear_motion::get_position()
{
    double axis_pos = 0;
    std::cout << "get axis curent position" << "\n";
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);

    std::string extracted = resp.substr(resp.find_first_of(":") + 1, resp.find_first_of(",") - 1 - resp.find_first_of(":"));
    axis_pos = std::stod(extracted); // to double
    std::cout << "filter val : " << axis_pos << "\n";
    axis_last_position.push_front(axis_pos); // add to table
    std::cout << "value added to table " << axis_last_position.front() << "\n";
    return axis_pos;
}
/**
 * @brief
 *
 */
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_home()
{
    auto command = axis_cmds.find("home");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move home reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}
/**
 * @brief
 *
 */
double ph_linear_motion::get_speed()
{
    double speed = 0;
    std::cout << "get axis curent spped" << "\n";
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error" << "\n";
        return 0;
    }
    //$110=800.000
    std::string extracted = [resp]() {
        size_t found1 = resp.find("$110=");
        size_t found2 = resp.find("$111=");
        if (found1 != std::string::npos && found2 != std::string::npos) {
            auto rep = resp.substr(found1 + 5, found2);
            std::cout << "Substring found " << rep << "\n";
            return rep;

        }
        else {
            std::cout << "Substring not found" << "\n";
            return std::string("0");
        }
    }(); // Note the added semicolon here
    speed = std::stod(extracted); // to double
    std::cout << "filter val : " << speed << "\n";
    return speed;
}
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::set_speed(double_t new_val)
{
    std::cout << "set  axis curent spped" << "\n";
    auto command = axis_cmds.find("set_speed");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_val << '\n';
        std::string args = std::to_string(new_val);
        auto cmd = (command->second) + args;
        // X-new_val
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "move down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_up_to(double_t new_pos)
{
    std::cout << "moving up by " << new_pos << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = "-" + std::to_string(new_pos);
        auto cmd = (command->second) + args;
        // X-new_pos
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "move down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_down_to(double_t new_pos)
{
    std::cout << "moving down to " << new_pos << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = std::to_string(-new_pos);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "move down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}
/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_up_by(double_t steps)
{
    std::cout << "moving up by " << steps << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "move down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_down_by(double_t steps)
{

    std::cout << "moving down by " << steps << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(-steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        std::cout << "move down reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;

}




wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::move_center()
{
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move center reply received " << reply << '\n';
        std::string args = std::to_string(-_motion_axis_struct.max_travel);
        auto cmd = (command->second) + args;
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_linear_motion::unlock()
{
    auto command = axis_cmds.find("unlock");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}