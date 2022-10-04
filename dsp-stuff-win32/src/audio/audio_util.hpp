#pragma once

#include <algorithm>
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

inline void fillVector(
    std::vector<float>& v,
    int beginOffset,
    int size,
    float value
) {
    std::fill(
        v.begin() + beginOffset,
        v.begin() + beginOffset + size,
        value
    );
}

inline void copyVector(
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
