#pragma once

#include <gmp.h>

void find_generator_full_group(mpz_t g, const mpz_t p);
void find_generator_subgroup(mpz_t g, const mpz_t p, const mpz_t q);
