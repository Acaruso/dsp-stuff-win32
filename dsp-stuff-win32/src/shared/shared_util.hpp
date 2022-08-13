#pragma once

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <vector>

#include "shared_constants.hpp"

inline double getRand() {
    return rand() / (RAND_MAX + 1.0);
}

template <typename T>
inline std::string toHexString(const T& t) {
    std::stringstream ss;
    ss << "0x" << std::hex << t;
    return ss.str();
}

template <typename T>
inline std::wstring toHexStringW(const T& t) {
    std::wstringstream ss;
    ss << "0x" << std::hex << t;
    return ss.str();
}

inline std::vector<double> makeSineBuffer(size_t size) {
    std::vector<double> buffer(size, 0.0);

    double inc = twoPi / size;
    double cur = 0.0;

    for (size_t i = 0; i < buffer.size(); i++) {
        buffer[i] = sin(cur);
        cur += inc;
    }

    return buffer;
}

template <typename T>
bool inBounds(std::vector<T>& vec, unsigned i) {
    return i > 0 && i < vec.size();
}
