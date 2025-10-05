#pragma once
#include <stdexcept>

enum class Protocol {
    DH,
    Subgroup_DH,
    MQV
};

inline Protocol parse_protocol(std::string str)
{
    if (str == "dh") {
        return Protocol::DH;
    } else if (str == "subgroup") {
        return Protocol::Subgroup_DH;
    } else if (str == "mqv") {
        return Protocol::MQV;
    } else {
        throw std::invalid_argument("Unknown protocol: " + str);
    }
}