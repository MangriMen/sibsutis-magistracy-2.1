#pragma once

#include <gmp.h>

bool is_prime(const mpz_t n, int reps = 25);
void generate_safe_prime(mpz_t prime, unsigned int bits, gmp_randstate_t &state);
void generate_safe_prime_pair(mpz_t q, mpz_t p, unsigned int q_bits, gmp_randstate_t &state);