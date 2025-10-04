#pragma once

#include <gmp.h>

// Original DH protocol functions
void generate_private_key(mpz_t private_key, const mpz_t q);
void generate_public_key(mpz_t public_key, const mpz_t g, const mpz_t private_key, const mpz_t p);
void compute_shared_secret(mpz_t shared_secret, const mpz_t public_key, const mpz_t private_key, const mpz_t p);

// DH in subgroup functions
// Using the same functions as original DH, but with different parameters
