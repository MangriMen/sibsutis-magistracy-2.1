#pragma once
#include <stdexcept>

enum class Protocol {
    DH,
    MQV
};

inline Protocol parse_protocol(std::string str)
{
    if (str == "dh") {
        return Protocol::DH;
    } else if (str == "mqv") {
        return Protocol::MQV;
    } else {
        throw std::invalid_argument("Unknown protocol: " + str);
    }
}