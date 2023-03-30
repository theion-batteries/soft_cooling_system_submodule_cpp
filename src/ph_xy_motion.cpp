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
    std::cout << "creating linear axis client"
              << "\n";
    _motion_struct.ip = ip;
    _motion_struct.port = port;
    _motion_struct.timeout = timeout;
}
ph_xy_motion::~ph_xy_motion()
{
    if (_client != nullptr)
        delete _client;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::connect()
{

    if (_client != nullptr && _client->is_connected())
        return sub_success;
    std::cout << "connecting controller to axis server"
              << "\n";
    std::cout << "axis server ip:  " << _motion_struct.ip << "\n";
    _client = new sockpp::tcp_connector({_motion_struct.ip, _motion_struct.port});
    _client->set_non_blocking();

    // Implicitly creates an inet_address from {host,port}
    // and then tries the connection.
    if (!_client->is_connected())
    {
        std::cerr << "Error connecting to axis server at "
                  << sockpp::inet_address(_motion_struct.ip, _motion_struct.port)
                  << " -> " << _client->last_error_str();
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    std::cout << "Created a connection from " << _client->address() << "\n";
    std::cout << "Created a connection to " << _client->peer_address() << "\n";
    // Set a timeout for the responses
    if (!_client->read_timeout(std::chrono::seconds(5)))
    {
        std::cerr << "Error setting timeout on TCP stream: "
                  << _client->last_error_str() << "\n";
        axisReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    axisReady = true;
    return wgm_feedbacks::enum_sub_sys_feedback::sub_success;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::disconnect()
{
    axisReady = false;
    if (_client->close())
        return sub_success;
    return sub_error;
}

std::string ph_xy_motion::sendDirectCmd(std::string cmd)
{
    if (_client == nullptr)
        return "not connected";
    if (blocking)
        _client->set_non_blocking(false);

    std::cout << "sending  command " << cmd << "\n";
    cmd = cmd + "\r\n";

    if (_client->write(cmd) != ssize_t(std::string(cmd).length()))
    {
        std::cout << "Error writing to the TCP stream: "
                  << _client->last_error_str() << "\n";
    }
    return waitForResponse();
}

std::string ph_xy_motion::waitForResponse()
{
    // std::cout << "awaiting server response" << "\n";
    auto start = std::chrono::steady_clock::now();
    while (_client->is_connected())
    {

        char Strholder[5012];

        ssize_t n = _client->read_n(&Strholder, sizeof(Strholder));
        if (n > 0)
        {
            //           std::cout << "n bytes received: " << n << "\n";
            incoming_data = Strholder;
            incoming_data.resize(n);
            //            std::cout << "server replied : " << incoming_data << "\n";
            break;
        }
        else
        {
            // std::cout << "no server response, retry " << n << "\n";
            incoming_data = "NA";
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
            if (duration >= _motion_struct.timeout)
            {
                std::cout << "no response within a timeout of " << duration << " seconds, "
                          << "aborting.."
                          << "\n";
                break;
            }
            continue;
        }
    }
    blocking = true;
    _client->set_non_blocking(true);

    return incoming_data;
}

bool ph_xy_motion::getStatus()
{
    return axisReady;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::unlock()
{
    auto command = axis_cmds.find("unlock");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::pause()
{
    auto command = axis_cmds.find("pause");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::resume()
{
    auto command = axis_cmds.find("resume");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "unlock reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::home_all()
{
    _client->set_non_blocking(false);
    auto command = axis_cmds.find("homeAll");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move home reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

std::string ph_xy_motion::get_settings()
{
    std::cout << "get axis curent speed"
              << "\n";
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error"
                  << "\n";
        return "NA";
    }
    return resp;
}

///////////////// linear apis ////////////////////
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_linear_Center_position(const double new_target)
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
    _client->set_non_blocking(false);
    double axis_pos = 0;
    std::cout << "get axis curent position"
              << "\n";
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);

    //    std::cout<<" received response "<<resp<<"\n";

    std::string extracted = resp.substr(resp.find("MPos:") + 5);
    extracted = extracted.substr(0, extracted.find_first_of(","));
    try
    {
        axis_pos = std::stod(extracted); // to double
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception caught in getting position " << __FILE__ << " " << __LINE__ << " " << e.what() << "\n";
    }
    _client->set_non_blocking(true);

    //    std::cout << "filter val : " << axis_pos << "\n";
    return axis_pos;
}

/**
 * @brief
 *
 */
double ph_xy_motion::get_linear_speed()
{
    double speed = 0;
    std::cout << "get axis curent spped"
              << "\n";
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (!resp.find("ok"))
    {
        std::cout << "missing ok, error"
                  << "\n";
        return 0;
    }
    //$110=800.000
    std::string extracted = [resp]()
    {
        size_t found1 = resp.find("$110=");
        size_t found2 = resp.find("$111=");
        if (found1 != std::string::npos && found2 != std::string::npos)
        {
            auto rep = resp.substr(found1 + 5, found2);
            std::cout << "Substring found " << rep << "\n";
            return rep;
        }
        else
        {
            std::cout << "Substring not found"
                      << "\n";
            return std::string("0");
        }
    }();                          // Note the added semicolon here
    speed = std::stod(extracted); // to double
    std::cout << "filter val : " << speed << "\n";
    return speed;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_Xspeed(const double_t new_val)
{
    std::cout << "set  axis curent spped"
              << "\n";
    auto command = axis_cmds.find("set_Xspeed");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << new_val << '\n';
        std::string args = std::to_string(new_val);
        auto cmd = (command->second) + args;
        // X-new_val
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
            return sub_success;
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_to(const double new_position)
{
    if (new_position < 0)
    {
        if (move_down_to(abs(new_position)) == sub_success)
            return sub_success;
        return sub_error;
    }
    else
    {
        if (move_down_to(abs(new_position)) == sub_success)
            return sub_success;
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
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move home reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_up_to(const double_t new_pos)
{
    std::cout << "moving up by " << new_pos << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = "-" + std::to_string(new_pos);
        auto cmd = (command->second) + args;
        // X-new_pos
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        std::cout << "move down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_down_to(const double_t new_pos)
{
    std::cout << "moving down to " << new_pos << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << new_pos << '\n';
        std::string args = std::to_string(-new_pos);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
        {
            std::cout << "move down reply received " << reply << '\n';
             while (true)
            {
            auto position = get_xy_position();
                if(abs(position.first)== abs(new_pos)) {
                    break;
                }
                std::cout << position.first << " " << abs(new_pos) << "\n";
            }
            return sub_success;
        }
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_up_by(const double_t steps)
{
    std::cout << "moving up by " << steps << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
            return sub_success;
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
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_down_by(const double_t steps)
{

    std::cout << "moving down by " << steps << "\n";
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << steps << '\n';
        std::string args = std::to_string(-steps);
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        std::cout << "move down reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::move_center()
{
    auto command = axis_cmds.find("move");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "move center reply received " << reply << '\n';
        std::string args = std::to_string(-_motion_struct.phead_start_angle);
        auto cmd = (command->second) + args;
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

/**
 * @param degree
 */

double ph_xy_motion::convert_degree_to_mm(const double degree)
{
    return turn_length * (degree / 360);
}

///////////////// rotation apis ////////////////////

/**
 * @brief TODO: change the function call
 *
 * @param new_position
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_to(const double degree)
{
    if (degree < 0)
    {
        if (rotate_down_to(abs(degree)) == sub_success)
            return sub_success;
     }
    else
    {
        if (rotate_up_to(abs(degree)) == sub_success)
            return sub_success;
     }
    std::cout<<" Error in rotating to  "<<degree<<"\n";
    return sub_error;
}

/**
 * @brief
 *
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_home()
{
    auto command = axis_cmds.find("homeY");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "rotate home reply received " << reply << '\n';
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_up_to(const double_t degree)
{
    std::cout << "rotating up to" << degree << "\n";
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << degree << '\n';
        std::string args = "-" + std::to_string(convert_degree_to_mm(degree));
        auto cmd = (command->second) + args;
        // X-new_pos
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
        {
            std::cout << "rotate up reply received " << reply << '\n';
             while (true)
            {
            auto position = get_xy_position();
                if(abs(position.second)== abs(degree)) {
                    break;
                }
                std::cout << position.second << " " << abs(degree) << "\n";
            }
            return sub_success;
        }
        std::cout << "rotate up reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_down_to(const double_t degree)
{
    std::cout << "rotate down to " << degree << "\n";
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << degree << '\n';
        std::string args = std::to_string(convert_degree_to_mm(-degree));
        auto cmd = (command->second) + args;
        // X-Steps
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
        {
            std::cout << "rotate down reply received " << reply << '\n';
             while (true)
            {
            auto position = get_xy_position();
                if(abs(position.second)== abs(degree)) {
                    break;
                }
                std::cout << position.second << " " << abs(degree) << "\n";
            }
            return sub_success;
        }
        return sub_error;
    }
    return sub_error;
}

/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_up_by(const double_t steps)
{
    return sub_error;
}

/**
 * @brief
 *
 * @param steps
 */
wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_down_by(const double_t steps)
{
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::rotate_center()
{
    auto command = axis_cmds.find("rotate");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << "rotate center reply received " << reply << '\n';
        std::string args = std::to_string(-_motion_struct.phead_start_angle);
        auto cmd = (command->second) + args;
        if (reply.find("ok") != std::string::npos)
            return sub_success;
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
    _client->set_non_blocking(false);

    double axis_pos = 0;
    std::cout << "get axis curent position"
              << "\n";
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    std::string first = resp.substr(resp.find_first_of(",") + 1, resp.find_first_of(",") + 10); // from , to ,+10
    std::string extracted = first.substr(0, first.find_first_of(",") - 1);
    // std::string extracted = resp.substr(first, resp.find_first_of(",") - 1 - resp.find_first_of(","));
    axis_pos = std::stod(extracted); // to double
    std::cout << "filter val : " << axis_pos << "\n";
    return axis_pos;
}

double ph_xy_motion::get_rotation_speed()
{
    _client->set_non_blocking(false);
    double speed = 0;
    std::cout << "get axis curent spped"
              << "\n";
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';

    auto resp = sendDirectCmd(command->second);
    if (resp.find("ok") == std::string::npos)
    {
        std::cout << "missing ok, error"
                  << "\n";
        return 0;
    }
    //$110=800.000
    std::string extracted = [resp]()
    {
        size_t found1 = resp.find("$111=");
        size_t found2 = resp.find("$112=");
        if (found1 != std::string::npos && found2 != std::string::npos)
        {
            auto rep = resp.substr(found1 + 5, found2);
            //            std::cout << "Substring found " << rep << "\n";
            return rep;
        }
        else
        {
            std::cout << "Substring not found"
                      << "\n";
            return std::string("0");
        }
    }();                          // Note the added semicolon here
    speed = std::stod(extracted); // to double
    std::cout << "filter val : " << speed << "\n";
    return speed;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_Yspeed(const double_t new_val)
{
    std::cout << "set  axis curent spped"
              << "\n";
    auto command = axis_cmds.find("set_Yspeed");
    if (command != axis_cmds.end())
    {
        std::cout << "sending command: " << command->second << " args: " << new_val << '\n';
        std::string args = std::to_string(new_val);
        auto cmd = (command->second) + args;
        // X-new_val
        auto reply = sendDirectCmd(cmd);
        if (reply.find("ok") != std::string::npos)
            return sub_success;
        std::cout << "rotate down reply received " << reply << '\n';
        return sub_error;
    }
    return sub_error;
}

wgm_feedbacks::enum_sub_sys_feedback ph_xy_motion::set_rotation_Center_position(const double new_target)
{
    _motion_struct.phead_start_angle = new_target;
    return sub_success;
}

// combined

std::pair<double, double> ph_xy_motion::get_xy_position()
{
    _client->set_non_blocking(false);

    double axis_pos = 0;
    double rot_pos = 0;
    std::cout << "get x,y curent position   123456 "
              << "\n";
    auto command = axis_cmds.find("get_position");
    std::cout << "sending command: " << command->second << '\n';
    std::string resp = sendDirectCmd(command->second);
    std::string extracted = resp.substr(resp.find_first_of(",") + 1, resp.length()); // to extract second part
    std::string x_pos = resp.substr(resp.find_first_of(":") + 1, resp.find_first_of(",") - 1 - resp.find_first_of(":"));
    try
    {
        axis_pos = std::stod(x_pos); // to double
        std::cout << "filter x val : " << axis_pos << "\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception caught in getting x position " << __FILE__ << " " << __LINE__ << " " << e.what() << "\n";
    }
    std::string y_pos = extracted.substr(0, extracted.find_first_of(",") - 1);
    try
    {
        rot_pos = std::stod(y_pos); // to double
        std::cout << "filter y val : " << rot_pos << "\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception caught in getting y position " << __FILE__ << " " << __LINE__ << " " << e.what() << "\n";
    }
    // add to vector
    positionXY.push_back(std::make_pair(axis_pos, rot_pos));
    _client->set_non_blocking(true);

    return std::make_pair(axis_pos, rot_pos);
}
std::pair<double, double> ph_xy_motion::get_xy_velocity()
{
    _client->set_non_blocking(false);

    double speedX = 0;
    double speedY = 0;
    std::cout << "get both axis speed"
              << "\n";
    auto command = axis_cmds.find("get_setting");
    std::cout << "sending command: " << command->second << '\n';
    auto resp = sendDirectCmd(command->second);
    //$110=600.000
    //$111=300.000
    std::string extracted = [resp]()
    {
        size_t foundX = resp.find("$110=");
        size_t foundY = resp.find("$111=");
        size_t foundZ = resp.find("$112=");
        if (foundX != std::string::npos && foundY != std::string::npos && foundZ != std::string::npos)
        {
            std::cout << " found X, Y and Z"
                      << "\n";
            auto X = resp.substr(foundX + 5, foundY);
            auto Y = resp.substr(foundY + 5, foundZ);
            std::string pair = X + "," + Y;
            return pair;
        }
        else
        {
            std::cout << "x,Y and Z not found"
                      << "\n";
            return std::string("0,0");
        }
    }(); // Note the added semicolon here to execute it directly
    try
    {
        auto Xstr = extracted.substr(0, extracted.find_first_of(",") - 1);
        auto Ystr = extracted.substr(extracted.find_first_of(",") + 1, extracted.length());
        speedX = std::stod(Xstr); // to double
        speedY = std::stod(Ystr); // to double
        std::cout << "filter x val : " << speedX << "\n";
        std::cout << "filter y val : " << speedY << "\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception caught in getting x, y velocity " << __FILE__ << " " << __LINE__ << " " << e.what() << "\n";
    }
    _client->set_non_blocking(true);
    return std::make_pair(speedX, speedY);
}

void ph_xy_motion::setModeBlocking(bool setblockingMode)
{
    if (setblockingMode)
        blocking = false;
}