#pragma once

#include "protocol.h"
#include <string>

void run_server(const std::string& params_path, Protocol protocol);

void run_dh_server(const std::string& params_path);
void run_subgroup_dh_server(const std::string& params_path);
void run_mqv_server(const std::string& params_path);
