#pragma once

#include <gmp.h>
#include <string>

struct DHParams
{
  mpz_t p; // Prime field
  mpz_t q; // Prime order of subgroup
  mpz_t g; // Generator
};

void init_dh_params(DHParams &params);
void clear_dh_params(DHParams &params);
void generate_params(DHParams &params, unsigned int q_bits = 256);

void save_params_to_file(const DHParams &params, const std::string &filename);
void load_params_from_file(DHParams &params, const std::string &filename);
