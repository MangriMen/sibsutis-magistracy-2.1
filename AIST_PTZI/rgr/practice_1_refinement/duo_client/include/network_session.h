#pragma once

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <gmp.h>

class NetworkSession {
public:
    NetworkSession();
    ~NetworkSession();

    void start_server(unsigned port = DEFAULT_PORT);
    void connect_to_server(const std::string& server_ip, unsigned port = DEFAULT_PORT);
    bool send_value(const mpz_t value);
    bool receive_value(mpz_t value);

    int send_data(const std::vector<uint8_t>& data);
    int receive_data(std::vector<uint8_t>& out, size_t size);

private:
#ifdef _WIN32
    SOCKET serverSocket;
    SOCKET clientSocket;
    WSADATA wsaData;
#else
    int serverSocket;
    int clientSocket;
#endif

    static const unsigned DEFAULT_PORT = 12345;

    NetworkSession(const NetworkSession&) = delete;
    NetworkSession& operator=(const NetworkSession&) = delete;
};
