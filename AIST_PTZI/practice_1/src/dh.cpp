#include "dh.h"

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>

#include "utils.h"

void generate_private_key(mpz_t private_key, const mpz_t q)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    gmp_randseed_ui(state, static_cast<unsigned long int>(seed));

    // Generate random number between 1 and q-1
    mpz_urandomm(private_key, state, q);
    mpz_add_ui(private_key, private_key, 1);

    gmp_randclear(state);
}

void generate_public_key(mpz_t public_key, const mpz_t g,
    const mpz_t private_key, const mpz_t p)
{
    mpz_powm(public_key, g, private_key, p);
}

void compute_shared_secret(mpz_t shared_secret, const mpz_t public_key,
    const mpz_t private_key, const mpz_t p)
{
    mpz_powm(shared_secret, public_key, private_key, p);
}
