#include <chrono>
#include <fstream>
#include "dh_params.h"
#include "utils.h"
#include "generators.h"

void init_dh_params(DHParams &params)
{
  mpz_init(params.p);
  mpz_init(params.q);
  mpz_init(params.g);
}

void clear_dh_params(DHParams &params)
{
  mpz_clear(params.p);
  mpz_clear(params.q);
  mpz_clear(params.g);
}

void generate_params(DHParams &params, unsigned int p_bits, unsigned int q_bits)
{
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, std::chrono::system_clock::now().time_since_epoch().count());

  mpz_t temp;
  mpz_init(temp);

  // Generate q
  do
  {
    mpz_urandomb(params.q, state, q_bits);
    mpz_setbit(params.q, q_bits - 1); // Ensure proper bit length
  } while (!is_prime(params.q));

  // Generate p = 2q + 1
  mpz_mul_ui(params.p, params.q, 2);
  mpz_add_ui(params.p, params.p, 1);

  while (!is_prime(params.p))
  {
    do
    {
      mpz_urandomb(params.q, state, q_bits);
      mpz_setbit(params.q, q_bits - 1);
    } while (!is_prime(params.q));

    mpz_mul_ui(params.p, params.q, 2);
    mpz_add_ui(params.p, params.p, 1);
  }

  // Find generator for full group
  find_generator_full_group(params.g, params.p);

  mpz_clear(temp);
  gmp_randclear(state);
}

void save_params_to_file(const DHParams &params, const std::string &filename)
{
  std::ofstream file(filename);
  if (!file.is_open())
  {
    throw std::runtime_error("Could not open file for writing");
  }

  file << mpz_get_str(nullptr, 16, params.p) << std::endl;
  file << mpz_get_str(nullptr, 16, params.q) << std::endl;
  file << mpz_get_str(nullptr, 16, params.g) << std::endl;

  file.close();
}

void load_params_from_file(DHParams &params, const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    throw std::runtime_error("Could not open file for reading");
  }

  std::string line;
  std::getline(file, line);
  mpz_set_str(params.p, line.c_str(), 16);

  std::getline(file, line);
  mpz_set_str(params.q, line.c_str(), 16);

  std::getline(file, line);
  mpz_set_str(params.g, line.c_str(), 16);

  file.close();
}