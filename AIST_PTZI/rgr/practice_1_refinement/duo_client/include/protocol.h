#pragma once
#include <stdexcept>

enum class Protocol {
    DH,
    MQV,
    MQV_SHA256_SALSA20
};

inline Protocol parse_protocol(std::string str)
{
    if (str == "dh") {
        return Protocol::DH;
    } else if (str == "mqv") {
        return Protocol::MQV;
    } else if (str == "mqv-sha256-salsa20") {
        return Protocol::MQV_SHA256_SALSA20;
    } else {
        throw std::invalid_argument("Unknown protocol: " + str);
    }
}