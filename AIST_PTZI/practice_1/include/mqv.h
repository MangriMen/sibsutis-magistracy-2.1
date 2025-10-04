#pragma once

#include <gmp.h>
#include "dh_params.h"

// MQV protocol functions
struct MQVKeyPair
{
  mpz_t private_key;
  mpz_t public_key;
};

void init_mqv_keypair(MQVKeyPair &keypair);
void clear_mqv_keypair(MQVKeyPair &keypair);
void generate_mqv_keypair(MQVKeyPair &keypair, const DHParams &params);
void compute_mqv_shared_secret(mpz_t shared_secret,
                               const MQVKeyPair &static_keypair,
                               const mpz_t ephemeral_private,
                               const mpz_t ephemeral_public_mine,
                               const mpz_t ephemeral_public_theirs,
                               const mpz_t static_public_theirs,
                               const DHParams &params);