/**
 * @file ph_xy_motion.h
 * @author sami dhiab
 * @version 0.1
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "ph_xy_motion.h"

#include <thread>
#include <mutex>
#include <string.h>

ph_xy_motion::ph_xy_motion(std::string ip, uint16_t port, const uint16_t timeout)
{
    std::cout << "creating linear axis client" << std::endl;
    _motion_struct.ip = ip;
    _motion_struct.port = port;
    _motion_struct.timeout = timeout;

}
ph_xy_motion::~ph_xy_motion()
{
    if (_client != nullptr) delete _client;

}


wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::connect()
{
    std::cout << "connecting controller to axis server" << std::endl;
    std::cout << "axis server ip:  " << _motion_struct.ip << std::endl;
    _client = new sockpp::tcp_connector({ _motion_struct.ip, _motion_struct.port });
    _client->set_non_blocking();

    // Implicitly creates an inet_address from {host,port}
    // and then tries the connection.
    if (!_client->is_connected()) {
        std::cerr << "Error connecting to axis server at "
            << sockpp::inet_address(_motion_struct.ip, _motion_struct.port)
            << " -> " << _client->last_error_str();
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    std::cout << "Created a connection from " << _client->address() << std::endl;
    std::cout << "Created a connection to " << _client->peer_address() << std::endl;
    // Set a timeout for the responses
    if (!_client->read_timeout(std::chrono::seconds(5))) {
        std::cerr << "Error setting timeout on TCP stream: "
            << _client->last_error_str() << std::endl;
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    axisReady = true;
    return wgm_feedbacks::enum_sub_sys_feedback::sub_success;

}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::disconnect()
{
    axisReady = false;
    if (_client->close()) return sub_success;
    return sub_error;
}


std::string ph_xy_motion::sendDirectCmd(std::string cmd)
{
    if (_client == nullptr) return "not connected";
    std::cout << "sending linear axis command " << cmd << std::endl;
    cmd = cmd + "\r\n";

    if (_client->write(cmd) != ssize_t(std::string(cmd).length())) {
        std::cout << "Error writing to the TCP stream: "
            << _client->last_error_str() << std::endl;
    }
    return waitForResponse();
}
std::string  ph_xy_motion::waitForResponse()
{
    std::cout << "awaiting server response" << std::endl;
    auto start = std::chrono::steady_clock::now();
    while (_client->is_connected())
    {

        char Strholder[5012];

        ssize_t n = _client->read_n(&Strholder, sizeof(Strholder));
        if (n > 0)
        {
            std::cout << "n bytes received: " << n << std::endl;
            incoming_data = Strholder;
            incoming_data.resize(n);
            std::cout << "server replied : " << incoming_data << std::endl;
            break;
        }
        else
        {
            std::cout << "no server response, retry " << n << std::endl;
            incoming_data = "NA";
            long long timeout = 10;
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
            if (duration >= timeout)
            {
                std::cout << "no response within a timeout of " << duration << " seconds, " << "aborting.." << std::endl;
                break;
            }
            continue;
        }

    }
    return incoming_data;
}

bool ph_xy_motion::getStatus()
{
    return axisReady;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::unlock()
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::pause()
{
    auto command = axis_cmds.find("pause");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::resume()
{
    auto command = axis_cmds.find("resume");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::home_all()
{
    auto command = axis_cmds.find("homeAll");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move home reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;

}
std::string ph_xy_motion::get_settings()
{
    std::cout << "get axis curent speed" << std::endl;
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error" << std::endl;
        return "NA";
    }
    return resp;
}
///////////////// linear apis ////////////////////
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_linear_Center_position(double new_target)
{
    _motion_struct.max_travel = new_target;
    return sub_success;
}
/**
 * @brief
 *
 * @return double
 */
double ph_xy_motion::get_linear_position()
{
    double axis_pos = 0;
    std::cout << "get axis curent position" << std::endl;
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);

    std::string extracted = resp.substr(resp.find_first_of(":") + 1, resp.find_first_of(",") - 1 - resp.find_first_of(":"));
    axis_pos = std::stod(extracted); // to double
    std::cout << "filter val : " << axis_pos << std::endl;
    return axis_pos;
}

/**
 * @brief
 *
 */
double ph_xy_motion::get_linear_speed()
{
    double speed = 0;
    std::cout << "get axis curent spped" << std::endl;
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error" << std::endl;
        return 0;
    }
    //$110=800.000
    std::string extracted = [resp]() {
        size_t found1 = resp.find("$110=");
        size_t found2 = resp.find("$111=");
        if (found1 != std::string::npos && found2 != std::string::npos) {
            auto rep = resp.substr(found1 + 5, found2);
            std::cout << "Substring found " << rep << std::endl;
            return rep;

        }
        else {
            std::cout << "Substring not found" << std::endl;
            return std::string("0");
        }
    }(); // Note the added semicolon here
    speed = std::stod(extracted); // to double
    std::cout << "filter val : " << speed << std::endl;
    return speed;
}
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_Xspeed(double_t new_val)
{
    std::cout << "set  axis curent spped" << std::endl;
    auto command = axis_cmds.find("set_Xspeed");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_val << '\n';
        std::string args = std::to_string(new_val);
        auto cmd = (command->second) + args;
        // X-new_val
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}


/**
 * @brief TODO: change the function call
 *
 * @param new_position
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_to(double new_position)
{
    if (new_position < 0)
    {
        if (move_down_to(abs(new_position)) == sub_success) return sub_success;
        return sub_error;
    }
    else
    {
        if (move_down_to(abs(new_position)) == sub_success) return sub_success;
        return sub_error;

    }

    return sub_error;
}


/**
 * @brief
 *
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_home()
{
    auto command = axis_cmds.find("homeX");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move home reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_up_to(double_t new_pos)
{
    std::cout << "moving up by " << new_pos << std::endl;
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_down_to(double_t new_pos)
{
    std::cout << "moving down to " << new_pos << std::endl;
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_up_by(double_t steps)
{
    std::cout << "moving up by " << steps << std::endl;
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_down_by(double_t steps)
{

    std::cout << "moving down by " << steps << std::endl;
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

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_center()
{
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move center reply received " << reply << '\n';
        std::string args = std::to_string(-_motion_struct.phead_start_angle);
        auto cmd = (command->second) + args;
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}




///////////////// rotation apis ////////////////////


/**
 * @brief TODO: change the function call
 *
 * @param new_position
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_to(double new_position)
{
    if (new_position < 0)
    {
        if (rotate_down_to(abs(new_position)) == sub_success) return sub_success;
        return sub_error;
    }
    else
    {
        if (rotate_down_to(abs(new_position)) == sub_success) return sub_success;
        return sub_error;

    }

    return sub_error;
}


/**
 * @brief
 *
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_home()
{
    auto command = axis_cmds.find("homeY");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "rotate home reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_up_to(double_t new_pos)
{
    std::cout << "moving up by " << new_pos << std::endl;
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = "-" + std::to_string(new_pos);
        auto cmd = (command->second) + args;
        // X-new_pos
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_down_to(double_t new_pos)
{
    std::cout << "moving down to " << new_pos << std::endl;
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = std::to_string(-new_pos);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}
/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_up_by(double_t steps)
{
    std::cout << "moving up by " << steps << std::endl;
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_down_by(double_t steps)
{

    std::cout << "moving down by " << steps << std::endl;
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(-steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        std::cout << "rotate down reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;

}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_center()
{
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "rotate center reply received " << reply << '\n';
        std::string args = std::to_string(-_motion_struct.phead_start_angle);
        auto cmd = (command->second) + args;
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;
}



/**
 * @brief
 *
 * @return double
 */
double ph_xy_motion::get_rotation_position()
{
    double axis_pos = 0;
    std::cout << "get axis curent position" << std::endl;
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    std::string first = resp.substr(resp.find_first_of(",") + 1, resp.find_first_of(",") + 10); // from , to ,+10
    std::string extracted = first.substr(0, first.find_first_of(",") - 1);
    //std::string extracted = resp.substr(first, resp.find_first_of(",") - 1 - resp.find_first_of(","));
    axis_pos = std::stod(extracted); // to double
    std::cout << "filter val : " << axis_pos << std::endl;
    return axis_pos;
}
double ph_xy_motion::get_rotation_speed()
{
    double speed = 0;
    std::cout << "get axis curent spped" << std::endl;
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error" << std::endl;
        return 0;
    }
    //$110=800.000
    std::string extracted = [resp]() {
        size_t found1 = resp.find("$111=");
        size_t found2 = resp.find("$112=");
        if (found1 != std::string::npos && found2 != std::string::npos) {
            auto rep = resp.substr(found1 + 5, found2);
            std::cout << "Substring found " << rep << std::endl;
            return rep;

        }
        else {
            std::cout << "Substring not found" << std::endl;
            return std::string("0");
        }
    }(); // Note the added semicolon here
    speed = std::stod(extracted); // to double
    std::cout << "filter val : " << speed << std::endl;
    return speed;
}
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_Yspeed(double_t new_val)
{
    std::cout << "set  axis curent spped" << std::endl;
    auto command = axis_cmds.find("set_Yspeed");
    if (command != axis_cmds.end()) {
        std::cout << "sending command: " << command->second << " args: " << new_val << '\n';
        std::string args = std::to_string(new_val);
        auto cmd = (command->second) + args;
        // X-new_val
        auto reply = sendDirectCmd(cmd);
        if (reply == "ok") return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_rotation_Center_position(double new_target)
{
    _motion_struct.phead_start_angle = new_target;
    return sub_success;
}
