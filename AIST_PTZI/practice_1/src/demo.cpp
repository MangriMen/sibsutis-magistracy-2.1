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
#include "mqv.h"
#include "prime.h"
#include "utils.h"

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

void demo_mqv(const std::string& params_path)
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

    demo_dh(multiplicative_params_path);
    std::cout << std::endl;
    demo_subgroup_dh(cyclic_params_path);
    std::cout << std::endl;
    demo_mqv(cyclic_params_path);

    return EXIT_SUCCESS;
}