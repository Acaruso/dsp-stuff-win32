#pragma once


#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

#include "src/audio/audio_constants.hpp"

const unsigned scale = (1 << 23) - 1;

// inline unsigned scaleSignal(double sig) {
//     double f = ((sig * 0.5) + 0.5) * scale;
//     unsigned u = (unsigned)f << 8;
//     return u;
// }

inline unsigned scaleSignal(float sig) {
    float f = ((sig * 0.5f) + 0.5f) * scale;
    unsigned u = (unsigned)f << 8;
    return u;
}

inline unsigned mstosamps(float ms) {
    return (unsigned)(ms * samplesPerMs);
}

inline double getTime(unsigned long sampleCounter) {
    return (double)(sampleCounter) * secondsPerSample;
}

inline void fillBuffer(
    std::vector<float>& data,
    int beginOffset,
    int size,
    float value
) {
    std::fill(
        data.begin() + beginOffset,
        data.begin() + beginOffset + size,
        value
    );
}

inline void copyBuffer(
    std::vector<float>& data,
    int sourceBeginOffset,
    int size,
    int destOffset
) {
    std::copy(
        data.begin() + sourceBeginOffset, 
        data.begin() + sourceBeginOffset + size,
        data.begin() + destOffset
    );
}

// windows.h defines `min` and `max` macros which cause issues
#undef min

inline bool isDenormal(float f) {
    return (
        f != 0 
        && (std::fabsf(f) < std::numeric_limits<float>::min())
    );
}

inline void printIfDenormal(float f, std::string s) {
    if (isDenormal(f)) {
        std::cout << "denormal: " << s << std::endl;
    }
}
