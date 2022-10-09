#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"

inline void makeAHRWavetable(
    std::vector<float>& wavetable,
    float a,
    float h,
    float r
) {
    int s_a = mstosamps(a);
    int s_h = mstosamps(h);
    int s_r = mstosamps(r);

    int size = s_a + s_h + s_r;

    float aDelta = 1.0f / (float)s_a;
    float rDelta = 1.0f / (float)s_r;

    float linearSig = 0.0f;
    float sig = 0.0f;

    wavetable.resize(size, 0.0f);

    for (int i = 0; i < size; i++) {
        if (i < s_a) {
            linearSig += aDelta;
            sig = sqrt(linearSig);
        } else if (i < s_a + s_h) {
            sig = 1.0f;
        } else if (i < s_a + s_h + s_r) {
            linearSig -= rDelta;
            sig = linearSig * linearSig;
        }

        wavetable[i] = sig;
    }
}
