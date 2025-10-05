#pragma once

#include <gmp.h>

void find_multiplicative_group_generator(mpz_t g, const mpz_t q, const mpz_t p);
void find_cyclic_subgroup_generator(mpz_t g, const mpz_t q, const mpz_t p, unsigned long seed);
