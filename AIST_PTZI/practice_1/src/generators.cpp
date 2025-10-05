#include "generators.h"

#include <chrono>
#include <iostream>

void find_multiplicative_group_generator(mpz_t g, const mpz_t q, const mpz_t p)
{
    mpz_t temp;
    mpz_init(temp);

    for (mpz_set_ui(g, 2); mpz_cmp(g, p) < 0; mpz_add_ui(g, g, 1)) { // g^q mod p != 1
        mpz_powm(temp, g, q, p);
        if (mpz_cmp_ui(temp, 1) != 0) { // g is a generator
            break;
        }
    }

    mpz_clear(temp);
}

void find_cyclic_subgroup_generator(mpz_t g, const mpz_t q, const mpz_t p, unsigned long seed)
{
    mpz_t r, temp;
    mpz_inits(r, temp, NULL);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, seed);

    mpz_sub_ui(temp, p, 1);
    mpz_divexact(temp, temp, q); // temp = (p-1)/q

    do {
        mpz_urandomm(r, state, p); // r in [0, p-1]
        mpz_add_ui(r, r, 1); // r in [1, p-1]
        mpz_powm(g, r, temp, p); // g = r^((p-1)/q) mod p
    } while (mpz_cmp_ui(g, 1) == 0); // Repeat if g == 1 to find a valid subgroup generator

    mpz_clears(r, temp, NULL);
    gmp_randclear(state);
}