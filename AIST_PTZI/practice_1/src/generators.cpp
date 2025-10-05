#include <chrono>
#include "generators.h"

void find_multiplicative_group_generator(mpz_t g, const mpz_t p)
{
  mpz_t temp;
  mpz_init(temp);

  // Try successive integers starting from 2
  mpz_set_ui(g, 2);
  while (mpz_cmp(g, p) < 0)
  {
    mpz_powm_ui(temp, g, 2, p);
    if (mpz_cmp_ui(temp, 1) != 0)
    {
      mpz_sub_ui(temp, p, 1);
      mpz_powm(temp, g, temp, p);
      if (mpz_cmp_ui(temp, 1) == 0)
      {
        break;
      }
    }
    mpz_add_ui(g, g, 1);
  }

  mpz_clear(temp);
}

void find_cyclic_subgroup_generator(mpz_t g, const mpz_t q, const mpz_t p)
{
  mpz_t r, temp;
  mpz_init(r);
  mpz_init(temp);

  // Initialize random state
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, std::chrono::system_clock::now().time_since_epoch().count());

  mpz_sub_ui(temp, p, 1);
  mpz_divexact(temp, temp, q); // temp = (p-1)/q

  do
  {
    // Choose random r between 1 and p-1
    mpz_urandomm(r, state, p);
    mpz_powm(g, r, temp, p);
  } while (mpz_cmp_ui(g, 1) == 0);

  gmp_randclear(state);
  mpz_clear(r);
  mpz_clear(temp);
}