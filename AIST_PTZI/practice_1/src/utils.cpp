#include "utils.h"
#include <iostream>

bool is_prime(const mpz_t n, int reps)
{
  return mpz_probab_prime_p(n, reps) > 0;
}

void print_number(const char *label, const mpz_t num)
{
  std::cout << label << ": " << mpz_get_str(nullptr, 16, num) << std::endl;
}