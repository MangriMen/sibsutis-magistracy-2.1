#include <chrono>
#include <gmp.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

const int DEFAULT_PORT = 12345;
const int BUFFER_SIZE = 4096;

std::string mpz_to_string(const mpz_t num);
void string_to_mpz(mpz_t num, const std::string& str);

bool send_mpz(SOCKET sock, const mpz_t num);
bool receive_mpz(SOCKET sock, mpz_t num);

class NetworkSession {
public:
    NetworkSession()
        : serverSocket(INVALID_SOCKET)
        , clientSocket(INVALID_SOCKET)
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~NetworkSession()
    {
        if (clientSocket != INVALID_SOCKET)
            closesocket(clientSocket);
        if (serverSocket != INVALID_SOCKET)
            closesocket(serverSocket);
        WSACleanup();
    }

    void start_server(unsigned port = DEFAULT_PORT)
    {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed");
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("Bind failed");
        }

        if (listen(serverSocket, 1) == SOCKET_ERROR) {
            throw std::runtime_error("Listen failed");
        }

        std::cout << "Server is listening on port " << port << std::endl;

        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Accept failed");
        }

        std::cout << "Client connected." << std::endl;
    }

    void connect_to_server(const std::string& server_ip)
    {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed");
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(DEFAULT_PORT);
        inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("Connection failed");
        }

        std::cout << "Connected to server." << std::endl;
    }

    bool send_value(const mpz_t value)
    {
        return send_mpz(clientSocket, value);
    }

    bool receive_value(mpz_t value)
    {
        return receive_mpz(clientSocket, value);
    }

private:
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKET clientSocket;
};
