#pragma once

#include <chrono>
#include <gmp.h>
#include <rdtsc.h>
#include <vector>

// If use_rdtsc = true, measure processor ticks (rdtsc).
// else - time in microseconds.
template <typename Func>
double measure_time(Func&& func, bool use_rdtsc = true)
{
    if (use_rdtsc) {
        auto start = CC();
        func();
        auto end = CC();
        double cycles = static_cast<double>(end - start);
        return cycles;
    } else {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        double micros = std::chrono::duration<double, std::micro>(end - start).count();
        return micros;
    }
}

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