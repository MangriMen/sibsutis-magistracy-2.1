#include "server.h"
#include "dh.h"
#include "dh_params.h"
#include "measure.h"
#include "mqv.h"
#include "network_session.h"
#include "print.h"
#include <iostream>

const int NAME_WIDTH = 24;
const int CYCLES_WIDTH = 20;

void run_server(const std::string& params_path, Protocol protocol)
{
    switch (protocol) {
    case Protocol::DH:
        run_dh_server(params_path);
        break;
    case Protocol::Subgroup_DH:
        run_subgroup_dh_server(params_path);
        break;
    case Protocol::MQV:
        run_mqv_server(params_path);
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