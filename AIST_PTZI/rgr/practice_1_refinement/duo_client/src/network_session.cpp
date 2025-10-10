#include "network_session.h"
#include "network.h"

NetworkSession::NetworkSession()
    : serverSocket(INVALID_SOCKET_VALUE)
    , clientSocket(INVALID_SOCKET_VALUE)
{
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
}

NetworkSession::~NetworkSession()
{
    if (clientSocket != INVALID_SOCKET_VALUE)
        CLOSE_SOCKET(clientSocket);
    if (serverSocket != INVALID_SOCKET_VALUE)
        CLOSE_SOCKET(serverSocket);

#ifdef _WIN32
    WSACleanup();
#endif
}

void NetworkSession::start_server(unsigned port)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET_VALUE) {
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
#ifdef _WIN32
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<const char*>(&opt), sizeof(opt))
        < 0) {
#else
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
#endif
        CLOSE_SOCKET(serverSocket);
        throw std::runtime_error("Setsockopt failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        CLOSE_SOCKET(serverSocket);
        throw std::runtime_error("Bind failed");
    }

    if (listen(serverSocket, 1) < 0) {
        CLOSE_SOCKET(serverSocket);
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server is listening on port " << port << std::endl;

    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(serverSocket);
        throw std::runtime_error("Accept failed");
    }

    std::cout << "Client connected." << std::endl;
}

void NetworkSession::connect_to_server(const std::string& server_ip, unsigned port)
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET_VALUE) {
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr) <= 0) {
        CLOSE_SOCKET(clientSocket);
        throw std::runtime_error("Invalid address");
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        CLOSE_SOCKET(clientSocket);
        throw std::runtime_error("Connection failed");
    }

    std::cout << "Connected to server." << std::endl;
}

bool NetworkSession::send_value(const mpz_t value)
{
    return send_mpz(clientSocket, value);
}

bool NetworkSession::receive_value(mpz_t value)
{
    return receive_mpz(clientSocket, value);
}