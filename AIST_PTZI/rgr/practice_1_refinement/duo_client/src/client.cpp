#include "client.h"
#include "dh.h"
#include "dh_params.h"
#include "measure.h"
#include "mqv.h"
#include "network_session.h"
#include "print.h"
#include "salsa20.h"
#include "sha256.h"
#include <fstream>
#include <helpers.h>
#include <iostream>
#include <cstring>

const int NAME_WIDTH = 24;
const int CYCLES_WIDTH = 20;

void run_client(const std::string& params_path, const std::string& server_ip, Protocol protocol, const std::string& file_to_send)
{
    switch (protocol) {
    case Protocol::DH:
        run_dh_client(params_path, server_ip);
        break;
    case Protocol::MQV:
        run_mqv_client(params_path, server_ip);
        break;
    case Protocol::MQV_SHA256_SALSA20:
        run_mqv_client_sha256_salsa20(params_path, server_ip, file_to_send);
        break;
    }
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

std::vector<uint8_t> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    return { std::istreambuf_iterator<char>(file), {} };
}

void run_mqv_client_sha256_salsa20(const std::string& params_path, const std::string& server_ip, const std::string& file_to_send)
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

        SHA256 sha256;
        std::string client_secret_str = mpz_get_str(nullptr, 16, client_secret);
        std::string hashed_secret = "";

        auto sha256_time = measure_time([&]() {
            hashed_secret = sha256(client_secret_str);
        });

        uint8_t key[32];
        uint8_t iv[8];
        auto derive_key_time = measure_time([&]() {
            derive_salsa20_key_iv(hashed_secret, key, iv);
        });

        // Init Salsa20
        ECRYPT_ctx ctx;
        auto ecrypt_init_time = measure_time([&]() {
            ECRYPT_init();
            ECRYPT_keysetup(&ctx, key, 256, 64); // keybits=256, ivbits=64 (8 bytes)
            ECRYPT_ivsetup(&ctx, iv);
        });

        std::cout << "\nClient's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, client_secret);
        std::cout << std::endl;

        std::cout << "\nClient's shared secret (sha-256):" << std::endl;
        std::cout << hashed_secret << std::endl;
        std::cout << std::endl;

        auto plain = readFile(file_to_send);

        std::vector<uint8_t> cipher;
        cipher.resize(plain.size());
        auto encrypt_time = measure_time([&]() {
            if (!plain.empty()) {
                ECRYPT_encrypt_bytes(&ctx, plain.data(), cipher.data(), static_cast<u32>(plain.size()));
            }
        });

        auto size = cipher.size();
        std::vector<uint8_t> size_in_bytes(sizeof(size));
        ::memcpy(size_in_bytes.data(), &size, sizeof(size));
        auto send_time = measure_time([&]() {
            if (session.send_data(size_in_bytes) < 0) { // Отправляем размер
                throw std::runtime_error("Failed to send encrypted data size");
            }
            if (session.send_data(cipher) < 0) {
                throw std::runtime_error("Failed to send encrypted data");
            }
        });

        print_performance_table(
            "MQV protocol (Client)",
            { { "Client static key", client_static_time },
                { "Client ephemeral key", client_ephemeral_time },
                { "Client shared secret", client_secret_time },
                { "SHA256", sha256_time },
                { "Derive key + iv", derive_key_time },
                { "Salsa20 init", ecrypt_init_time },
                { "Encrypt data", encrypt_time },
                { "Send data", send_time } },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "Encrypted data sent to server" << "(" << size << " bytes)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(ephemeral_private, ephemeral_public, server_ephemeral_public,
        server_static_public, client_secret, NULL);
}
