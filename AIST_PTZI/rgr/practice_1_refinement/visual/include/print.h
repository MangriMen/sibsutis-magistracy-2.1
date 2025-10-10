#pragma once

#include <chrono>
#include <gmp.h>
#include <vector>

std::string center(const std::string s, const int w);
std::string left(const std::string s, const int w);
std::string prd(const unsigned int x, const int width);

void print_number(const char* label, const mpz_t num);

void print_secrets(mpz_t alice_secret, mpz_t bob_secret);

void print_protocol_header(const std::string& protocol_name, int name_width,
    int cycles_width);
void print_performance_row(const std::string& name, unsigned int cycles,
    int name_width, int cycles_width);
void print_performance_table(
    const std::string& protocol_name,
    const std::vector<std::tuple<std::string, unsigned int>>& performance_data,
    int name_width, int cycles_width);