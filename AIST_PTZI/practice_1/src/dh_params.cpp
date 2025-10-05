#include <chrono>
#include <fstream>
#include "dh_params.h"
#include "utils.h"
#include "generators.h"
#include "prime.h"

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