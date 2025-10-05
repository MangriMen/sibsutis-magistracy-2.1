#pragma once

#include <gmp.h>

#include "dh_params.h"

struct MQVKeyPair {
    mpz_t private_key;
    mpz_t public_key;

    MQVKeyPair()
    {
        mpz_inits(private_key, public_key, nullptr);
    }

    ~MQVKeyPair()
    {
        mpz_clears(private_key, public_key, nullptr);
    }

    // Restrict copy operations
    MQVKeyPair(const MQVKeyPair&) = delete;
    MQVKeyPair& operator=(const MQVKeyPair&) = delete;

    // Allow move semantics
    MQVKeyPair(MQVKeyPair&& other) noexcept
    {
        mpz_init_set(private_key, other.private_key);
        mpz_init_set(public_key, other.public_key);
    }

    MQVKeyPair& operator=(MQVKeyPair&& other) noexcept
    {
        if (this != &other) {
            mpz_set(private_key, other.private_key);
            mpz_set(public_key, other.public_key);
        }
        return *this;
    }
};

void generate_mqv_keypair(MQVKeyPair& keypair, const DHParams& params);
void compute_mqv_shared_secret(mpz_t shared_secret,
    const MQVKeyPair& static_keypair,
    const mpz_t ephemeral_private,
    const mpz_t ephemeral_public_mine,
    const mpz_t ephemeral_public_theirs,
    const mpz_t static_public_theirs,
    const DHParams& params);
