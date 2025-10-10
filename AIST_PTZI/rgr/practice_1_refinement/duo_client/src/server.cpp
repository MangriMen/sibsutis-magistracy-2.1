#include "server.h"
#include "dh.h"
#include "dh_params.h"
#include "measure.h"
#include "mqv.h"
#include "network_session.h"
#include "print.h"
#include <fstream>
#include <helpers.h>
#include <iostream>
#include <salsa20.h>
#include <sha256.h>
#include <cstring>

const int NAME_WIDTH = 24;
const int CYCLES_WIDTH = 20;

void run_server(const std::string& params_path, Protocol protocol)
{
    switch (protocol) {
    case Protocol::DH:
        run_dh_server(params_path);
        break;
    case Protocol::MQV:
        run_mqv_server(params_path);
        break;
    case Protocol::MQV_SHA256_SALSA20:
        run_mqv_server_sha256_salsa20(params_path);
        break;
    }
}

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

void writeFile(const std::string& filename, const std::vector<uint8_t>& data)
{
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void run_mqv_server_sha256_salsa20(const std::string& params_path)
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

        // Derive key + iv
        SHA256 sha256;
        std::string server_secret_str = mpz_get_str(nullptr, 16, server_secret);
        std::string hashed_secret = "";

        auto sha256_time = measure_time([&]() {
            hashed_secret = sha256(server_secret_str);
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

        std::cout << "\nServer's shared secret:" << std::endl;
        mpz_out_str(stdout, 16, server_secret);
        std::cout << std::endl;

        std::cout << "\nServer's shared secret (sha-256):" << std::endl;
        std::cout << hashed_secret << std::endl;
        std::cout << std::endl;

        size_t size = 0;
        std::vector<uint8_t> size_in_bytes(sizeof(size));
        std::vector<uint8_t> cipher;

        auto receive_time = measure_time([&]() {
            if (!session.receive_data(size_in_bytes, sizeof(size))) {
                throw std::runtime_error("Failed to receive encrypted data");
            }
            ::memcpy(&size, size_in_bytes.data(), sizeof(size));

            if (!session.receive_data(cipher, size)) {
                throw std::runtime_error("Failed to receive encrypted data");
            }
        });

        // Дешифровка
        std::vector<uint8_t> plain;
        plain.resize(cipher.size());
        auto decrypt_time = measure_time([&]() {
            if (!cipher.empty()) {
                ECRYPT_decrypt_bytes(&ctx, cipher.data(), plain.data(), static_cast<u32>(cipher.size()));
            }
        });

        writeFile("received.txt", plain);

        print_performance_table(
            "MQV protocol (Server)",
            {
                { "Server static key", server_static_time },
                { "Server ephemeral key", server_ephemeral_time },
                { "Server shared secret", server_secret_time },
                { "SHA-256", sha256_time },
                { "Derive key + iv", derive_key_time },
                { "Salsa20 init", ecrypt_init_time },
                { "Receive", receive_time },
                { "Decrypt", decrypt_time },
            },
            NAME_WIDTH, CYCLES_WIDTH);

        std::cout << "Received " << "(" << plain.size() << " bytes)" << " (decrypted)" << std::endl
                  << "saved to received.txt"
                  << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    mpz_clears(ephemeral_private, ephemeral_public, client_ephemeral_public,
        client_static_public, server_secret, NULL);
}
