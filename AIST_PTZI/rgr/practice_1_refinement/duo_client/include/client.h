#pragma once

#include "protocol.h"
#include <string>

void run_client(const std::string& params_path, const std::string& server_ip, Protocol protocol, const std::string& file_to_send);

void run_dh_client(const std::string& params_path, const std::string& server_ip);
void run_mqv_client(const std::string& params_path, const std::string& server_ip);
void run_mqv_client_sha256_salsa20(const std::string& params_path, const std::string& server_ip, const std::string& file_to_send);
