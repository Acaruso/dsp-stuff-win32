#pragma once

#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include "shared_constants.hpp"

// get random double between 0.0 and 1.0
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

inline std::vector<double> makeSineBuffer(unsigned size) {
    std::vector<double> buffer(size, 0.0);

    double inc = twoPi / size;
    double cur = 0.0;

    for (unsigned i = 0; i < buffer.size(); i++) {
        buffer[i] = sin(cur);
        cur += inc;
    }

    return buffer;
}

template <typename T>
bool inBounds(std::vector<T>& vec, unsigned i) {
    return i > 0 && i < vec.size();
}
