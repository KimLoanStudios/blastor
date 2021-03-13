#pragma once
#include <SFML/Network.hpp>
#include <cstring>
#include "types.hpp"
#include "config.hpp"
#include <iostream>

struct GameConfig {
    sf::IpAddress server_address;
    u16 server_port;
    std::string username;

    GameConfig() = delete;

    GameConfig(int argc, const char* argv[]) {
        std::cout << "Creating GameConfig!";

        std::cout << "XD\n" << std::flush;
        // Parse server address
        int server_address_pos = -1;
        for(int i  = 0; i < argc; i++) {
            if(strcmp(argv[i], "--server-address") == 0) {
                server_address_pos = i;
                break;
            }
        }


        if(server_address_pos != -1) {
            if(server_address_pos + 1 == argc) {
                throw std::runtime_error("No argument after --server--address!");
            }

            parse_server_address(argv[server_address_pos + 1]);
        } else {
            server_address = sf::IpAddress(127, 0, 0, 1);
            server_port = SERVER_PORT;
        }

        // Parse username
        int username_pos = -1;
        for(int i = 0; i < argc; i++) {
            if(strcmp(argv[i], "--username") == 0) {
                username_pos = i;
                break;
            }
        }

        if(username_pos != -1) {
            if(username_pos + 1 == argc) {
                throw std::runtime_error("No argument after --username!");
            }

            username = std::string(argv[username_pos + 1]);
        } else {
            username = "default_user"; // TODO maybe random username?
        }
    }

    void parse_server_address(const char* addr_str) {
        int colon_pos = -1;
        for(int i = 0; addr_str[i] != 0; i++) {
            if(addr_str[i] == ':') {
                colon_pos = i;
                break;
            }
        }

        if(colon_pos == -1) {
            throw std::runtime_error("No port specified in server address!");
        }

        std::string ip_addr_str = std::string(addr_str).substr(0, colon_pos);

        if(colon_pos + 1 == (int)strlen(addr_str)) {
            throw std::runtime_error("Empty port number");
        }

        std::string port_str = std::string(addr_str).substr(colon_pos + 1);

        server_address = sf::IpAddress(ip_addr_str);
        server_port = atoi(port_str.c_str());
    }
};