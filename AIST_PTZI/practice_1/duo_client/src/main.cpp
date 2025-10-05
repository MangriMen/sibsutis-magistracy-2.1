#include <rdtsc.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "dh.h"
#include "dh_params.h"
#include "generators.h"
#include "measure.h"
#include "mqv.h"
#include "prime.h"
#include "print.h"

const int NAME_WIDTH = 24;
const int CYCLES_WIDTH = 20;
const int DEFAULT_PORT = 12345;
const int BUFFER_SIZE = 4096;

enum class Protocol {
    DH,
    SUBGROUP_DH,
    MQV
};

// Function to convert mpz_t to string for network transmission
std::string mpz_to_string(const mpz_t num)
{
    char* str = mpz_get_str(nullptr, 16, num);
    std::string result(str);
    free(str);
    return result;
}

// Function to convert string back to mpz_t
void string_to_mpz(mpz_t num, const std::string& str)
{
    mpz_set_str(num, str.c_str(), 16);
}

// Function to send mpz_t over socket
bool send_mpz(SOCKET sock, const mpz_t num)
{
    std::string str = mpz_to_string(num);
    str += "\n";
    return send(sock, str.c_str(), str.length(), 0) != SOCKET_ERROR;
}

// Function to receive mpz_t from socket
bool receive_mpz(SOCKET sock, mpz_t num)
{
    char buffer[BUFFER_SIZE];
    std::string str;
    int bytes;

    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        str += buffer;
        if (str.find('\n') != std::string::npos) {
            break;
        }
    }

    if (bytes <= 0)
        return false;

    str = str.substr(0, str.find('\n'));
    string_to_mpz(num, str);
    return true;
}

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

    void start_server()
    {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            throw std::runtime_error("Socket creation failed");
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(DEFAULT_PORT);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("Bind failed");
        }

        if (listen(serverSocket, 1) == SOCKET_ERROR) {
            throw std::runtime_error("Listen failed");
        }

        std::cout << "Server is listening on port " << DEFAULT_PORT << std::endl;

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

void run_dh_server(const std::string& params_path)
{
    NetworkSession session;
    session.start_server();

    std::cout << "Starting Diffie-Hellman key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t server_private, server_public, client_public, server_secret;
    mpz_inits(server_private, server_public, client_public, server_secret, NULL);

    try {
        auto server_key_time = measure_time([&]() {
            generate_private_key(server_private, params.q);
            generate_public_key(server_public, params.g, server_private, params.p);
        });

        if (!session.send_value(server_public)) {
            throw std::runtime_error("Failed to send public key");
        }

        if (!session.receive_value(client_public)) {
            throw std::runtime_error("Failed to receive client's public key");
        }

        auto server_secret_time = measure_time([&]() {
            compute_shared_secret(server_secret, client_public, server_private, params.p);
        });

        print_performance_table(
            "Diffie-Hellman protocol (Server)",
            { { "Server key", server_key_time },
                { "Server shared secret", server_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nServer's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, server_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(server_private, server_public, client_public, server_secret, NULL);
}

void run_subgroup_dh_server(const std::string& params_path)
{
    NetworkSession session;
    session.start_server();

    std::cout << "Starting Subgroup Diffie-Hellman key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t server_private, server_public, client_public, server_secret;
    mpz_inits(server_private, server_public, client_public, server_secret, NULL);

    try {
        auto server_key_time = measure_time([&]() {
            generate_private_key(server_private, params.q);
            generate_public_key(server_public, params.g, server_private, params.p);
        });

        if (!session.send_value(server_public)) {
            throw std::runtime_error("Failed to send public key");
        }

        if (!session.receive_value(client_public)) {
            throw std::runtime_error("Failed to receive client's public key");
        }

        auto server_secret_time = measure_time([&]() {
            compute_shared_secret(server_secret, client_public, server_private, params.p);
        });

        print_performance_table(
            "Subgroup Diffie-Hellman protocol (Server)",
            { { "Server key", server_key_time },
                { "Server shared secret", server_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nServer's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, server_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(server_private, server_public, client_public, server_secret, NULL);
}

void run_mqv_server(const std::string& params_path)
{
    NetworkSession session;
    session.start_server();

    std::cout << "Starting MQV key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t ephemeral_private, ephemeral_public, client_ephemeral_public;
    mpz_t client_static_public, server_secret;
    MQVKeyPair server_static_keypair;

    mpz_inits(ephemeral_private, ephemeral_public, client_ephemeral_public,
        client_static_public, server_secret, NULL);

    try {
        // Generate static key pair
        auto server_static_time = measure_time([&]() {
            generate_mqv_keypair(server_static_keypair, params);
        });

        // Generate ephemeral key pair
        auto server_ephemeral_time = measure_time([&]() {
            generate_private_key(ephemeral_private, params.q);
            generate_public_key(ephemeral_public, params.g, ephemeral_private, params.p);
        });

        // Exchange static public keys
        if (!session.send_value(server_static_keypair.public_key)) {
            throw std::runtime_error("Failed to send static public key");
        }
        if (!session.receive_value(client_static_public)) {
            throw std::runtime_error("Failed to receive client's static public key");
        }

        // Exchange ephemeral public keys
        if (!session.send_value(ephemeral_public)) {
            throw std::runtime_error("Failed to send ephemeral public key");
        }
        if (!session.receive_value(client_ephemeral_public)) {
            throw std::runtime_error("Failed to receive client's ephemeral public key");
        }

        // Compute shared secret
        auto server_secret_time = measure_time([&]() {
            compute_mqv_shared_secret(server_secret, server_static_keypair,
                ephemeral_private, ephemeral_public,
                client_ephemeral_public, client_static_public,
                params);
        });

        print_performance_table(
            "MQV protocol (Server)",
            { { "Server static key", server_static_time },
                { "Server ephemeral key", server_ephemeral_time },
                { "Server shared secret", server_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nServer's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, server_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(ephemeral_private, ephemeral_public, client_ephemeral_public,
        client_static_public, server_secret, NULL);
}

void run_dh_client(const std::string& params_path, const std::string& server_ip)
{
    NetworkSession session;
    session.connect_to_server(server_ip);

    std::cout << "Starting Diffie-Hellman key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t client_private, client_public, server_public, client_secret;
    mpz_inits(client_private, client_public, server_public, client_secret, NULL);

    try {
        auto client_key_time = measure_time([&]() {
            generate_private_key(client_private, params.q);
            generate_public_key(client_public, params.g, client_private, params.p);
        });

        if (!session.receive_value(server_public)) {
            throw std::runtime_error("Failed to receive server's public key");
        }

        if (!session.send_value(client_public)) {
            throw std::runtime_error("Failed to send public key");
        }

        auto client_secret_time = measure_time([&]() {
            compute_shared_secret(client_secret, server_public, client_private, params.p);
        });

        print_performance_table(
            "Diffie-Hellman protocol (Client)",
            { { "Client key", client_key_time },
                { "Client shared secret", client_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nClient's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, client_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(client_private, client_public, server_public, client_secret, NULL);
}

void run_subgroup_dh_client(const std::string& params_path, const std::string& server_ip)
{
    NetworkSession session;
    session.connect_to_server(server_ip);

    std::cout << "Starting Subgroup Diffie-Hellman key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t client_private, client_public, server_public, client_secret;
    mpz_inits(client_private, client_public, server_public, client_secret, NULL);

    try {
        auto client_key_time = measure_time([&]() {
            generate_private_key(client_private, params.q);
            generate_public_key(client_public, params.g, client_private, params.p);
        });

        if (!session.receive_value(server_public)) {
            throw std::runtime_error("Failed to receive server's public key");
        }

        if (!session.send_value(client_public)) {
            throw std::runtime_error("Failed to send public key");
        }

        auto client_secret_time = measure_time([&]() {
            compute_shared_secret(client_secret, server_public, client_private, params.p);
        });

        print_performance_table(
            "Subgroup Diffie-Hellman protocol (Client)",
            { { "Client key", client_key_time },
                { "Client shared secret", client_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nClient's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, client_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(client_private, client_public, server_public, client_secret, NULL);
}

void run_mqv_client(const std::string& params_path, const std::string& server_ip)
{
    NetworkSession session;
    session.connect_to_server(server_ip);

    std::cout << "Starting MQV key exchange..." << std::endl;

    DHParams params;
    load_params_from_file(params, params_path);

    mpz_t ephemeral_private, ephemeral_public, server_ephemeral_public;
    mpz_t server_static_public, client_secret;
    MQVKeyPair client_static_keypair;

    mpz_inits(ephemeral_private, ephemeral_public, server_ephemeral_public,
        server_static_public, client_secret, NULL);

    try {
        // Generate static key pair
        auto client_static_time = measure_time([&]() {
            generate_mqv_keypair(client_static_keypair, params);
        });

        // Generate ephemeral key pair
        auto client_ephemeral_time = measure_time([&]() {
            generate_private_key(ephemeral_private, params.q);
            generate_public_key(ephemeral_public, params.g, ephemeral_private, params.p);
        });

        // Exchange static public keys
        if (!session.receive_value(server_static_public)) {
            throw std::runtime_error("Failed to receive server's static public key");
        }
        if (!session.send_value(client_static_keypair.public_key)) {
            throw std::runtime_error("Failed to send static public key");
        }

        // Exchange ephemeral public keys
        if (!session.receive_value(server_ephemeral_public)) {
            throw std::runtime_error("Failed to receive server's ephemeral public key");
        }
        if (!session.send_value(ephemeral_public)) {
            throw std::runtime_error("Failed to send ephemeral public key");
        }

        // Compute shared secret
        auto client_secret_time = measure_time([&]() {
            compute_mqv_shared_secret(client_secret, client_static_keypair,
                ephemeral_private, ephemeral_public,
                server_ephemeral_public, server_static_public,
                params);
        });

        print_performance_table(
            "MQV protocol (Client)",
            { { "Client static key", client_static_time },
                { "Client ephemeral key", client_ephemeral_time },
                { "Client shared secret", client_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "\nClient's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, client_secret);
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(ephemeral_private, ephemeral_public, server_ephemeral_public,
        server_static_public, client_secret, NULL);
}

void run_server(const std::string& params_path, Protocol protocol)
{
    switch (protocol) {
    case Protocol::DH:
        run_dh_server(params_path);
        break;
    case Protocol::SUBGROUP_DH:
        run_subgroup_dh_server(params_path);
        break;
    case Protocol::MQV:
        run_mqv_server(params_path);
        break;
    }
}

void run_client(const std::string& params_path, const std::string& server_ip, Protocol protocol)
{
    switch (protocol) {
    case Protocol::DH:
        run_dh_client(params_path, server_ip);
        break;
    case Protocol::SUBGROUP_DH:
        run_subgroup_dh_client(params_path, server_ip);
        break;
    case Protocol::MQV:
        run_mqv_client(params_path, server_ip);
        break;
    }
}

void print_usage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "  Server mode: duo_client.exe -s <protocol> <params_file>" << std::endl;
    std::cout << "  Client mode: duo_client.exe -c <protocol> <server_ip> <params_file>" << std::endl;
    std::cout << "Protocols:" << std::endl;
    std::cout << "  dh         - Diffie-Hellman" << std::endl;
    std::cout << "  subgroup   - Subgroup Diffie-Hellman" << std::endl;
    std::cout << "  mqv        - MQV protocol" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        print_usage();
        return 1;
    }

    std::string mode = argv[1];
    std::string protocol_str = argv[2];

    Protocol protocol;
    if (protocol_str == "dh") {
        protocol = Protocol::DH;
    } else if (protocol_str == "subgroup") {
        protocol = Protocol::SUBGROUP_DH;
    } else if (protocol_str == "mqv") {
        protocol = Protocol::MQV;
    } else {
        std::cerr << "Unknown protocol: " << protocol_str << std::endl;
        print_usage();
        return 1;
    }

    try {
        if (mode == "-s" && argc == 4) {
            // Server mode
            run_server(argv[3], protocol);
        } else if (mode == "-c" && argc == 5) {
            // Client mode
            run_client(argv[4], argv[3], protocol);
        } else {
            print_usage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}