#include <iostream>
#include <chrono>
#include <rdtsc.h>
#include "prime.h"
#include "dh_params.h"
#include "generators.h"
#include "utils.h"

void save_params(const mpz_t q, const mpz_t p, const mpz_t g, const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);
  mpz_set(params.q, q);
  mpz_set(params.p, p);
  mpz_set(params.g, g);

  save_params_to_file(params, params_path);
  clear_dh_params(params);
}

void demo_parameter_generation(const std::string &params_dir_path)
{

  std::cout << "Generating parameters..." << std::endl;

  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, std::chrono::system_clock::now().time_since_epoch().count());

  mpz_t q, p;
  mpz_init(q);
  mpz_init(p);

  unsigned int q_bits = 256;

  unsigned int start = CC();
  generate_safe_prime_pair(q, p, q_bits, state);
  unsigned int end = CC();
  unsigned int safe_prime_pairs_cycles = end - start;

  gmp_randclear(state);

  mpz_t multiplicative_group_g;
  mpz_init(multiplicative_group_g);
  start = CC();
  find_multiplicative_group_generator(multiplicative_group_g, p);
  end = CC();
  unsigned int multiplicative_cycles = end - start;

  mpz_t cyclic_group_g;
  mpz_init(cyclic_group_g);
  start = CC();
  find_cyclic_subgroup_generator(cyclic_group_g, q, p);
  end = CC();
  unsigned int cyclic_cycles = end - start;

  save_params(q, p, multiplicative_group_g, params_dir_path + "multiplicative_params.txt");
  save_params(q, p, cyclic_group_g, params_dir_path + "cyclic_params.txt");

  mpz_clear(q);
  mpz_clear(p);
  mpz_clear(multiplicative_group_g);
  mpz_clear(cyclic_group_g);

  const int name_width = 22;
  const int cycles_width = 12;
  std::cout << center("Name", name_width) << "|" << center("Cycles", cycles_width) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";
  std::cout << left("Safe prime generation", name_width) << "|" << prd(safe_prime_pairs_cycles, cycles_width) << "\n";
  std::cout << left("Multiplicative group", name_width) << "|" << prd(multiplicative_cycles, cycles_width) << "\n";
  std::cout << left("Cyclic group", name_width) << "|" << prd(cyclic_cycles, cycles_width) << "\n";
}

int main()
{
  const std::string params_dir_path = "";

  demo_parameter_generation(params_dir_path);

  return EXIT_SUCCESS;
}