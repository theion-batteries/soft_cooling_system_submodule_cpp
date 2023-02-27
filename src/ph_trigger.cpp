#include "ph_trigger.h"

ph_trigger::ph_trigger(std::string ip, uint16_t port)
{
    _trigger_struct.ip = ip;
    _trigger_struct.port = port;

}

std::string ph_trigger::sendDirectCmd(std::string cmd)
{
    if (_client == nullptr) return "not connected";
    std::cout << "sending command " << cmd << std::endl;
    cmd = cmd + "\r\n";

    if (_client->write(cmd) != ssize_t(std::string(cmd).length())) {
        std::cout << "Error writing to the TCP stream: "
            << _client->last_error_str() << std::endl;
    }
    return waitForResponse();
}
std::string ph_trigger::waitForResponse()
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
            if(duration >= timeout)
            {
            std::cout << "no response within a timeout of "<<duration<< " seconds, " <<"aborting.."<< std::endl;
            break;
            } 
            continue;
        }

    }
    return incoming_data;
}
wgm_feedbacks::enum_sub_sys_feedback ph_trigger::connect()
{
    std::cout << "connecting controller to trigger server" << std::endl;
    std::cout << "trigger server ip:  " << _trigger_struct.ip << std::endl;
    _client = new sockpp::tcp_connector({ _trigger_struct.ip, _trigger_struct.port });
    // Implicitly creates an inet_address from {host,port}
    // and then tries the connection.
    if (!_client->is_connected()) {
        std::cerr << "Error connecting to trigger server at "
            << sockpp::inet_address(_trigger_struct.ip, _trigger_struct.port)
            << " -> " << _client->last_error_str();
        triggerReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    std::cout << "Created a connection from " << _client->address() << std::endl;
    std::cout << "Created a connection to " << _client->peer_address() << std::endl;
    // Set a timeout for the responses
    if (!_client->read_timeout(std::chrono::seconds(5))) {
        std::cerr << "Error setting timeout on TCP stream: "
            << _client->last_error_str() << std::endl;
        triggerReady = false;
        return wgm_feedbacks::enum_sub_sys_feedback::sub_error;
    }
    triggerReady = true;
    return wgm_feedbacks::enum_sub_sys_feedback::sub_success;

}
wgm_feedbacks::enum_sub_sys_feedback ph_trigger::disconnect()
{
    triggerReady = false;
    if (_client->close()) return sub_success;
    return sub_error;

}
wgm_feedbacks::enum_sub_sys_feedback ph_trigger::turn_on()
{
    auto command = trigger_cmds.find("turn_on");
    if (command != trigger_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << " reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;

}
wgm_feedbacks::enum_sub_sys_feedback ph_trigger::turn_off()
{
    auto command = trigger_cmds.find("turn_on");
    if (command != trigger_cmds.end()) {
        std::cout << "sending command: " << command->second << '\n';
        auto reply = sendDirectCmd(command->second);
        std::cout << " reply received " << reply << '\n';
        if (reply == "ok") return sub_success;
        return sub_error;
    }
    return sub_error;

}


bool ph_trigger::getStatus()
{
    return triggerReady;
}
