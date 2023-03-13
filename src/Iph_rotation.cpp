/**
 * @file Iph_rotation.h
 * @author sami dhiab
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Iph_rotation.h"
Iph_rotation::Iph_rotation(/* args */)
{
}

Iph_rotation::~Iph_rotation()
{
}



/**
 * @brief
 *
 * @param cmd
 * @param client
 * @param args
 */
void Iph_rotation::sendCmd(std::string& cmd, sockpp::tcp_connector* client, std::string args)
{
    if (client->write(cmd + args) != ssize_t(std::string(cmd + args).length())) {
        std::cerr << "Error writing to the TCP stream: "
            << client->last_error_str() << "\n";
    }
    std::cout << "command " << cmd + args << " sent" << "\n";
}

