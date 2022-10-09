#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"

inline void makeAHRWavetable(
    std::vector<float>& wavetable,
    int size,     // desired size of wavetable in samples
    float a,      // a h r times in ms
    float h,
    float r
) {
    int s_a = mstosamps(a);
    int s_h = mstosamps(h);
    int s_r = mstosamps(r);

    int audioSize = s_a + s_h + s_r;

    float ratio = (float)size / (float)audioSize;

    int s_a_wt = s_a * ratio;
    int s_h_wt = s_h * ratio;
    int s_r_wt = s_r * ratio;

    float aDelta = 1.0f / (float)s_a_wt;
    float rDelta = 1.0f / (float)s_r_wt;

    float linearSig = 0.0f;
    float sig = 0.0f;

    wavetable.resize(size, 0.0f);

    for (int i = 0; i < size; i++) {
        if (i < s_a_wt) {
            linearSig += aDelta;
            sig = sqrt(linearSig);
        } else if (i < s_a_wt + s_h_wt) {
            sig = 1.0f;
        } else if (i < s_a_wt + s_h_wt + s_r_wt) {
            linearSig -= rDelta;
            sig = linearSig * linearSig;
        }

        wavetable[i] = sig;
    }
}
