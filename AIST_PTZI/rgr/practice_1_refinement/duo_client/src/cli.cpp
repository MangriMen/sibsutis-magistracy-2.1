#include "cli.h"
#include <cstring>
#include <iostream>
#include <stdexcept>

RunMode parse_run_mode(std::string mode)
{
    if (mode == "-s") {
        return RunMode::Server;
    } else if (mode == "-c") {
        return RunMode::Client;
    } else {
        throw std::invalid_argument("Unknown run mode: " + mode);
    }
}

Args parse_args(int argc, char* argv[])
{
    RunMode mode = parse_run_mode(argv[1]);
    Protocol protocol = parse_protocol(argv[2]);
    std::string params_path = argv[3];
    std::string file_to_send = "";
    std::string server_ip = "";

    if (mode == RunMode::Client) {

        if (argc < 5 || strlen(argv[4]) == 0) {
            server_ip = "";
            throw std::invalid_argument("Server IP is empty");
        }
        server_ip = argv[4];

        if (protocol == Protocol::MQV_SHA256_SALSA20) {
            if (argc < 6 || strlen(argv[5]) == 0) {
                file_to_send = "";
                throw std::invalid_argument("File to send is empty");
            }

            file_to_send = argv[5];
        }
    }

    Args args = {
        mode,
        protocol,
        params_path,
        server_ip,
        file_to_send
    };

    return args;
}

std::string filename(std::string& path)
{
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string filename = path.substr(lastSlash + 1);
        return filename;
    } else {
        return path;
    }
}

void print_usage(std::string& path)
{
    const std::string cleaned_name = filename(path);

    std::cout << "Usage:" << std::endl;
    std::cout << "  Server mode: " << cleaned_name << " -s <protocol> <params_file>" << std::endl;
    std::cout << "  Client mode: " << cleaned_name << " -c <protocol> <params_file> <server_ip> [file_to_send (only mqv-sha256-salsa20)]" << std::endl;
    std::cout << "Protocols:" << std::endl;
    std::cout << "  dh                 - Diffie-Hellman (subgroup depends on params)" << std::endl;
    std::cout << "  mqv                - MQV protocol" << std::endl;
    std::cout << "  mqv-sha256-salsa20 - MQV protocol with SHA256 and Salsa20" << std::endl;
}
