#pragma once

#include <cstdlib>

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
