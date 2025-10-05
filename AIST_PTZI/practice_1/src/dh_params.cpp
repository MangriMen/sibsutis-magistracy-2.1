#include "dh_params.h"

#include <chrono>
#include <fstream>

#include "generators.h"
#include "prime.h"
#include "utils.h"

void save_params_to_file(const DHParams& params, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing");
    }

    file << mpz_get_str(nullptr, 16, params.p) << std::endl;
    file << mpz_get_str(nullptr, 16, params.q) << std::endl;
    file << mpz_get_str(nullptr, 16, params.g) << std::endl;

    file.close();
}

void load_params_from_file(DHParams& params, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading");
    }

    std::string line;
    if (!std::getline(file, line))
        throw std::runtime_error("Failed to read p");
    if (mpz_set_str(params.p, line.c_str(), 16) != 0)
        throw std::runtime_error("Invalid p");

    if (!std::getline(file, line))
        throw std::runtime_error("Failed to read q");
    if (mpz_set_str(params.q, line.c_str(), 16) != 0)
        throw std::runtime_error("Invalid q");

    if (!std::getline(file, line))
        throw std::runtime_error("Failed to read g");
    if (mpz_set_str(params.g, line.c_str(), 16) != 0)
        throw std::runtime_error("Invalid g");

    file.close();
}