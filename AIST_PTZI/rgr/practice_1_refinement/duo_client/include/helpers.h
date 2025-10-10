#include "sha256.h"
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

// hex -> байты
static std::vector<uint8_t> hex_to_bytes(const std::string& hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        unsigned int byte = 0;
        std::istringstream iss(hex.substr(i, 2));
        iss >> std::hex >> byte;
        out.push_back(static_cast<uint8_t>(byte));
    }
    return out;
}

// Деривация key (32 байта) и iv (8 байт) из hex-строки SHA256
static void derive_salsa20_key_iv(const std::string& hashed_secret_hex, uint8_t key_out[32], uint8_t iv_out[8])
{
    auto hash_bytes = hex_to_bytes(hashed_secret_hex);
    if (hash_bytes.size() < 32)
        hash_bytes.resize(32, 0);
    for (size_t i = 0; i < 32; ++i)
        key_out[i] = hash_bytes[i];

    // iv = первые 8 байт SHA256(hashed_secret_hex + "IV")
    SHA256 sha;
    std::string iv_source = hashed_secret_hex + "IV";
    std::string iv_hash_hex = sha(iv_source);
    auto iv_hash_bytes = hex_to_bytes(iv_hash_hex);
    for (size_t i = 0; i < 8; ++i)
        iv_out[i] = iv_hash_bytes[i];
}
