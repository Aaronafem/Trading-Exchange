/**
 * @file helpers.cpp
 * @brief This file contains the little helper functions used elsewhere.
 */
#ifndef HELPERS_HPP
#define HELPERS_HPP
#pragma once
#include <random>

int get_random_int(int min, int max);  


inline uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}


#endif // HELPERS_HPP
