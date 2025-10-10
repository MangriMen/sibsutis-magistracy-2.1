#pragma once

#include "protocol.h"
#include <string>

enum class RunMode {
    Server,
    Client
};

struct Args {
    RunMode mode;
    Protocol protocol;
    std::string params_path;
    std::string server_ip;
};

RunMode parse_run_mode(std::string mode);
Args parse_args(int argc, char* argv[]);

void print_usage(std::string& path);