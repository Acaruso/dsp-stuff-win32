#pragma once

#include <string>
#include <sstream>

#include "src/audio/audio_constants.hpp"
#include "src/shared/audio_buffer.hpp"

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

inline void sumCopy(AudioBuffer& dest, AudioBuffer& source) {
    for (int i = 0; i < dest.size(); ++i) {
        dest[i] += source[i];
    }
}
