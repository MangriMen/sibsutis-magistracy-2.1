#pragma once

#include <gmp.h>

void print_number(const char *label, const mpz_t num);

std::string center(const std::string s, const int w);
std::string left(const std::string s, const int w);

std::string prd(const unsigned int x, const int width);
