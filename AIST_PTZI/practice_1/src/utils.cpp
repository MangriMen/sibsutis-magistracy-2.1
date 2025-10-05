#include "utils.h"

#include <rdtsc.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

/*! Center-aligns string within a field of width w. Pads with blank spaces
    to enforce alignment. */
std::string center(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size(); // count excess room to pad
    for (int i = 0; i < padding / 2; ++i)
        spaces << " ";
    ss << spaces.str() << s << spaces.str(); // format with padding
    if (padding > 0 && padding % 2 != 0) // if odd #, add 1 space
        ss << " ";
    return ss.str();
}

/*! Left-aligns string within a field of width w. Pads with blank spaces
    to enforce alignment. */
std::string left(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size(); // count excess room to pad
    for (int i = 0; i < padding; ++i)
        spaces << " ";
    ss << s << spaces.str(); // format with padding
    return ss.str();
}

/* Convert double to string with specified number of places after the decimal
   and left padding. */
std::string prd(const unsigned int x, const int width)
{
    std::stringstream ss;
    ss << std::right;
    ss.fill(' '); // fill space around displayed #
    ss.width(width); // set  width around displayed #
    ss << x;
    return ss.str();
}

void print_number(const char* label, const mpz_t num)
{
    std::cout << label << ": " << mpz_get_str(nullptr, 16, num) << std::endl;
}

void print_secrets(mpz_t alice_secret, mpz_t bob_secret)
{
    print_number("Alice's shared secret", alice_secret);
    print_number("  Bob's shared secret", bob_secret);
    std::cout << "Secrets match: "
              << (mpz_cmp(alice_secret, bob_secret) == 0 ? "Yes" : "No")
              << std::endl;
}

void print_protocol_header(const std::string& protocol_name, int name_width,
    int cycles_width)
{
    const std::string separator = std::string(name_width, '-') + "|"
        + std::string(cycles_width, '-');

    std::cout << center(protocol_name, name_width + cycles_width + 1) << "\n";
    std::cout << separator << "\n";
    std::cout << center("Name", name_width) << "|"
              << center("Cycles", cycles_width) << "\n";
    std::cout << separator << "\n";
}

void print_performance_row(const std::string& name, unsigned int cycles,
    int name_width, int cycles_width)
{
    std::cout << left(name, name_width) << "|" << prd(cycles, cycles_width)
              << "\n";
}

void print_performance_table(
    const std::string& protocol_name,
    const std::vector<std::tuple<std::string, unsigned int>>& performance_data,
    int name_width, int cycles_width)
{
    print_protocol_header(protocol_name, name_width, cycles_width);
    for (const auto& entry : performance_data) {
        print_performance_row(std::get<0>(entry), std::get<1>(entry),
            name_width, cycles_width);
    }
}