#pragma once

#include <gmp.h>

#include <string>

struct DHParams {
    mpz_t p; // Prime field
    mpz_t q; // Prime order of subgroup
    mpz_t g; // Generator

    DHParams()
    {
        mpz_inits(p, q, g, nullptr);
    }

    ~DHParams()
    {
        mpz_clears(p, q, g, nullptr);
    }

    // Restrict copying to avoid double cleanup
    DHParams(const DHParams&) = delete;
    DHParams& operator=(const DHParams&) = delete;

    // Allow move semantics
    DHParams(DHParams&& other) noexcept
    {
        mpz_init_set(p, other.p);
        mpz_init_set(q, other.q);
        mpz_init_set(g, other.g);
    }

    DHParams& operator=(DHParams&& other) noexcept
    {
        if (this != &other) {
            mpz_set(p, other.p);
            mpz_set(q, other.q);
            mpz_set(g, other.g);
        }
        return *this;
    }
};

void save_params_to_file(const DHParams& params, const std::string& filename);
void load_params_from_file(DHParams& params, const std::string& filename);
