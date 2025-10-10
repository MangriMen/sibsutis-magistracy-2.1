#pragma once

#include <chrono>
#include <gmp.h>
#include <rdtsc.h>

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
