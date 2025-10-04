#pragma once

#include <gmp.h>

bool is_prime(const mpz_t n, int reps = 25);
void print_number(const char *label, const mpz_t num);