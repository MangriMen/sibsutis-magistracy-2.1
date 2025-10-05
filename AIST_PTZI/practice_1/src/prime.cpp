#include "prime.h"

bool is_prime(const mpz_t n, int reps)
{
  return mpz_probab_prime_p(n, reps) > 0;
}

void generate_safe_prime(mpz_t prime, unsigned int bits, gmp_randstate_t &state)
{
  do
  {
    mpz_urandomb(prime, state, bits);
    mpz_nextprime(prime, prime);
    mpz_setbit(prime, bits - 1); // Ensure proper bit length
  } while (!is_prime(prime));
}

void generate_safe_prime_pair(mpz_t q, mpz_t p, unsigned int q_bits, gmp_randstate_t &state)
{
  do
  {
    // Generate q
    generate_safe_prime(q, q_bits, state);
    // Generate p = 2q + 1
    mpz_mul_ui(p, q, 2);
    mpz_add_ui(p, p, 1);
  } while (!is_prime(p));
}