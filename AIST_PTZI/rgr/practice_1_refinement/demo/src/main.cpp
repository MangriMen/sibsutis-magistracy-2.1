#include <rdtsc.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "dh.h"
#include "dh_params.h"
#include "generators.h"
#include "measure.h"
#include "mqv.h"
#include "prime.h"
#include "print.h"
#include "sha256.h"

const int NAME_WIDTH = 24;
const int CYCLES_WIDTH = 20;

void demo_dh(const std::string& params_path)
{

    mpz_t alice_private, alice_public, bob_private, bob_public, alice_secret,
        bob_secret;
    mpz_inits(alice_private, alice_public, bob_private, bob_public,
        alice_secret, bob_secret, NULL);

    DHParams params;
    // Load parameters
    load_params_from_file(params, params_path);

    // Keys
    auto alice_key_time = measure_time([&]() {
        generate_private_key(alice_private, params.q);
        generate_public_key(alice_public, params.g, alice_private, params.p);
    });
    auto bob_key_time = measure_time([&]() {
        generate_private_key(bob_private, params.q);
        generate_public_key(bob_public, params.g, bob_private, params.p);
    });

    // Shared secrets
    auto alice_shared_secret_time = measure_time([&]() {
        compute_shared_secret(alice_secret, bob_public, alice_private, params.p);
    });
    auto bob_shared_secret_time = measure_time([&]() {
        compute_shared_secret(bob_secret, alice_public, bob_private, params.p);
    });

    print_performance_table(
        "Diffie-Hellman protocol",
        { { "Alice key", alice_key_time },
            { "Bob key", bob_key_time },
            { "Alice shared secret", alice_shared_secret_time },
            { "Bob shared secret", bob_shared_secret_time } },
        NAME_WIDTH, CYCLES_WIDTH);
    std::cout << std::endl;
    print_secrets(alice_secret, bob_secret);

    // Cleanup
    mpz_clears(alice_private, alice_public, bob_private, bob_public,
        alice_secret, bob_secret, NULL);
}

void demo_subgroup_dh(const std::string& params_path)
{

    mpz_t alice_private, alice_public, bob_private, bob_public;
    mpz_t alice_secret, bob_secret;
    mpz_inits(alice_private, alice_public, bob_private, bob_public,
        alice_secret, bob_secret, NULL);

    DHParams params;
    // Load parameters
    load_params_from_file(params, params_path);

    // Keys
    auto alice_key_time = measure_time([&]() {
        generate_private_key(alice_private, params.q);
        generate_public_key(alice_public, params.g, alice_private, params.p);
    });
    auto bob_key_time = measure_time([&]() {
        generate_private_key(bob_private, params.q);
        generate_public_key(bob_public, params.g, bob_private, params.p);
    });

    // Shared secrets
    auto alice_secret_time = measure_time([&]() {
        compute_shared_secret(alice_secret, bob_public, alice_private, params.p);
    });
    auto bob_secret_time = measure_time([&]() {
        compute_shared_secret(bob_secret, alice_public, bob_private, params.p);
    });

    print_performance_table(
        "Subgroup Diffie-Hellman protocol",
        { { "Alice key", alice_key_time },
            { "Bob key", bob_key_time },
            { "Alice shared secret", alice_secret_time },
            { "Bob shared secret", bob_secret_time } },
        NAME_WIDTH, CYCLES_WIDTH);
    std::cout << std::endl;
    print_secrets(alice_secret, bob_secret);

    // Cleanup
    mpz_clears(alice_private, alice_public, bob_private, bob_public,
        alice_secret, bob_secret, NULL);
}

void demo_mqv(const std::string& params_path, bool should_print_info)
{

    mpz_t alice_ephemeral_private, alice_ephemeral_public;
    mpz_t bob_ephemeral_private, bob_ephemeral_public;
    mpz_t alice_secret, bob_secret;

    mpz_inits(alice_ephemeral_private, alice_ephemeral_public,
        bob_ephemeral_private, bob_ephemeral_public,
        alice_secret, bob_secret, NULL);

    DHParams params;
    // Load parameters
    load_params_from_file(params, params_path);

    MQVKeyPair alice_static, bob_static;
    // Static keys
    auto alice_static_time = measure_time([&]() { generate_mqv_keypair(alice_static, params); });
    auto bob_static_time = measure_time([&]() { generate_mqv_keypair(bob_static, params); });

    // Ephemeral keys
    auto alice_ephemeral_time = measure_time([&]() {
        generate_private_key(alice_ephemeral_private, params.q);
        generate_public_key(alice_ephemeral_public, params.g,
            alice_ephemeral_private, params.p);
    });
    auto bob_ephemeral_time = measure_time([&]() {
        generate_private_key(bob_ephemeral_private, params.q);
        generate_public_key(bob_ephemeral_public, params.g,
            bob_ephemeral_private, params.p);
    });

    // Shared secrets
    auto alice_secret_time = measure_time([&]() {
        compute_mqv_shared_secret(alice_secret, alice_static,
            alice_ephemeral_private, alice_ephemeral_public,
            bob_ephemeral_public, bob_static.public_key,
            params);
    });
    auto bob_secret_time = measure_time([&]() {
        compute_mqv_shared_secret(bob_secret, bob_static,
            bob_ephemeral_private, bob_ephemeral_public,
            alice_ephemeral_public, alice_static.public_key,
            params);
    });

    if (should_print_info) {
        print_performance_table(
            "MQV protocol",
            { { "Alice static keypair", alice_static_time },
                { "Bob static keypair", bob_static_time },
                { "Alice ephemeral key", alice_ephemeral_time },
                { "Bob ephemeral key", bob_ephemeral_time },
                { "Alice MQV shared secret", alice_secret_time },
                { "Bob MQV shared secret", bob_secret_time } },
            NAME_WIDTH, CYCLES_WIDTH);
        std::cout << std::endl;
        print_secrets(alice_secret, bob_secret);
    }

    mpz_clears(alice_ephemeral_private, alice_ephemeral_public,
        bob_ephemeral_private, bob_ephemeral_public,
        alice_secret, bob_secret, NULL);
}

void demo_mqv_sha256(const std::string& params_path, bool should_print_info)
{

    mpz_t alice_ephemeral_private, alice_ephemeral_public;
    mpz_t bob_ephemeral_private, bob_ephemeral_public;
    mpz_t alice_secret, bob_secret;

    mpz_inits(alice_ephemeral_private, alice_ephemeral_public,
        bob_ephemeral_private, bob_ephemeral_public,
        alice_secret, bob_secret, NULL);

    DHParams params;
    // Load parameters
    load_params_from_file(params, params_path);

    MQVKeyPair alice_static, bob_static;
    // Static keys
    auto alice_static_time = measure_time([&]() { generate_mqv_keypair(alice_static, params); });
    auto bob_static_time = measure_time([&]() { generate_mqv_keypair(bob_static, params); });

    // Ephemeral keys
    auto alice_ephemeral_time = measure_time([&]() {
        generate_private_key(alice_ephemeral_private, params.q);
        generate_public_key(alice_ephemeral_public, params.g,
            alice_ephemeral_private, params.p);
    });
    auto bob_ephemeral_time = measure_time([&]() {
        generate_private_key(bob_ephemeral_private, params.q);
        generate_public_key(bob_ephemeral_public, params.g,
            bob_ephemeral_private, params.p);
    });

    // Shared secrets
    auto alice_secret_time = measure_time([&]() {
        compute_mqv_shared_secret(alice_secret, alice_static,
            alice_ephemeral_private, alice_ephemeral_public,
            bob_ephemeral_public, bob_static.public_key,
            params);
    });
    auto bob_secret_time = measure_time([&]() {
        compute_mqv_shared_secret(bob_secret, bob_static,
            bob_ephemeral_private, bob_ephemeral_public,
            alice_ephemeral_public, alice_static.public_key,
            params);
    });

    SHA256 sha256;

    int iterations = should_print_info ? 1000 : 1;
    // Warmup
    // for (int i = 0; i < iterations; i++) {
    //     sha256("warmup string");
    //     sha256.reset();
    // }
    std::string alice_hash = "", bob_hash = "";

    double alice_hashed_time = 0, bob_hashed_time = 0;
    auto alice_secret_str = mpz_get_str(NULL, 16, alice_secret);
    auto bob_secret_str = mpz_get_str(NULL, 16, bob_secret);
    for (int i = 0; i < iterations; i++) {
        // Hash the shared secrets with SHA-256
        alice_hashed_time += measure_time([&]() {
            alice_hash = sha256(alice_secret_str);
            sha256.reset();
        });
        bob_hashed_time += measure_time([&]() {
            bob_hash = sha256(bob_secret_str);
            sha256.reset();
        });
    }
    alice_hashed_time /= iterations;
    bob_hashed_time /= iterations;

    if (should_print_info) {
        print_performance_table(
            "MQV protocol (SHA-256)",
            { { "Alice static keypair", alice_static_time },
                { "Bob static keypair", bob_static_time },
                { "Alice ephemeral key", alice_ephemeral_time },
                { "Bob ephemeral key", bob_ephemeral_time },
                { "Alice MQV shared secret", alice_secret_time },
                { "Bob MQV shared secret", bob_secret_time },
                { "Alice hashed secret", alice_hashed_time },
                { "Bob hashed secret", bob_hashed_time } },
            NAME_WIDTH, CYCLES_WIDTH);
        std::cout << std::endl;
        print_secrets(alice_secret, bob_secret);

        std::cout << "Alice hashed secret: " << alice_hash << "\n";
        std::cout << "Bob hashed secret:   " << bob_hash << "\n";
    }

    mpz_clears(alice_ephemeral_private, alice_ephemeral_public,
        bob_ephemeral_private, bob_ephemeral_public,
        alice_secret, bob_secret, NULL);
}

int main()
{
    const std::string params_dir_path = "";

    const std::string multiplicative_params_path
        = params_dir_path + "multiplicative_params.txt";
    const std::string cyclic_params_path = params_dir_path + "cyclic_params.txt";

    demo_mqv(cyclic_params_path, true);
    demo_mqv_sha256(cyclic_params_path, true);

    double mqv_time = 0, mqv_sha256_time = 0;
    int iterations = 500;
    for (int i = 0; i < iterations; i++) {
        mqv_time += measure_time([&]() {
            demo_mqv(cyclic_params_path, false);
        });
        mqv_sha256_time += measure_time([&]() {
            demo_mqv_sha256(cyclic_params_path, false);
        });
    }
    mqv_time /= iterations;
    mqv_sha256_time /= iterations;

    print_performance_table(
        "Performance",
        {
            { "MQV", mqv_time },
            { "MQV SHA-256", mqv_sha256_time },
            { "Difference", mqv_sha256_time - mqv_time },
        },
        NAME_WIDTH, CYCLES_WIDTH);

    return EXIT_SUCCESS;
}