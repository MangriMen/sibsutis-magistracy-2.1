#include <chrono>
#include <iostream>

#include "dh_params.h"
#include "generators.h"
#include "measure.h"
#include "prime.h"
#include "print.h"

void save_params(const mpz_t q, const mpz_t p, const mpz_t g,
    const std::string& params_path)
{
    DHParams params;
    mpz_set(params.q, q);
    mpz_set(params.p, p);
    mpz_set(params.g, g);

    save_params_to_file(params, params_path);
}

void demo_parameter_generation(const std::string& params_dir_path)
{
    std::cout << "Generating parameters..." << std::endl;

    gmp_randstate_t state;
    gmp_randinit_default(state);
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    gmp_randseed_ui(state, seed);

    mpz_t q, p;
    mpz_t multiplicative_group_g, cyclic_group_g;
    mpz_inits(q, p, multiplicative_group_g, cyclic_group_g, NULL);

    unsigned int q_bits = 256;

    auto safe_prime_pairs_cycles = measure_time([&]() {
        generate_safe_prime_pair(q, p, q_bits, state);
    });
    gmp_randclear(state);

    auto multiplicative_cycles = measure_time([&]() {
        find_multiplicative_group_generator(multiplicative_group_g, q, p);
    });

    auto cyclic_seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto cyclic_cycles = measure_time([&]() {
        find_cyclic_subgroup_generator(cyclic_group_g, q, p, static_cast<unsigned long>(cyclic_seed));
    });

    save_params(q, p, multiplicative_group_g, params_dir_path + "multiplicative_params.txt");
    save_params(q, p, cyclic_group_g, params_dir_path + "cyclic_params.txt");

    mpz_clears(q, p, multiplicative_group_g, cyclic_group_g, NULL);

    const int name_width = 22;
    const int cycles_width = 12;

    print_performance_table(
        "Parameter generation performance",
        { { "Safe prime generation", safe_prime_pairs_cycles },
            { "Multiplicative group", multiplicative_cycles },
            { "Cyclic group", cyclic_cycles } },
        name_width, cycles_width);
}

int main()
{
    const std::string params_dir_path = "";

    demo_parameter_generation(params_dir_path);

    return EXIT_SUCCESS;
}