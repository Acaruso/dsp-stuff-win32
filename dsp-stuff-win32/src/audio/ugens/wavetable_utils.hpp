#pragma once

#include <cmath>
#include <random>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

inline std::vector<float>* makeAHRWavetable(
    int sizeSamps,     // desired size of wavetable in samples
    AHRData ahrData
) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    int sizeToFillSamps = sizeSamps - 1;

    int attackSamps = mstosamps(ahrData.a);
    int holdSamps = mstosamps(ahrData.h);
    int releaseSamps = mstosamps(ahrData.r);

    int audioSizeSamps = attackSamps + holdSamps + releaseSamps;

    // float audioToWtRatio = (float)sizeToFillSamps / (float)audioSizeSamps;
    float audioToWtRatio = 0.0f;

    if (audioSizeSamps <= sizeToFillSamps) {
        audioToWtRatio = 1.0f;
    } else {
        audioToWtRatio = (float)sizeToFillSamps / (float)audioSizeSamps;
    }

    int attackSampsWt = attackSamps * audioToWtRatio;
    int holdSampsWt = holdSamps * audioToWtRatio;
    int releaseSampsWt = releaseSamps * audioToWtRatio;

    float attackDelta = 1.0f / (float)attackSampsWt;
    float releaseDelta = 1.0f / (float)releaseSampsWt;

    float linearSig = 0.0f;
    float sig = 0.0f;

    for (int i = 0; i < sizeToFillSamps; ++i) {
        if (i < attackSampsWt) {
            linearSig += attackDelta;
            sig = sqrt(linearSig);
        } else if (i < attackSampsWt + holdSampsWt) {
            sig = 1.0f;
        } else if (i < attackSampsWt + holdSampsWt + releaseSampsWt) {
            linearSig -= releaseDelta;
            sig = linearSig * linearSig;
        }

        (*wavetable)[i] = sig;
    }

    return wavetable;
}

inline void makeSinWavetable(
    std::vector<float>& wavetable,
    int sizeSamps
) {
    wavetable.resize(sizeSamps, 0.0f);

    int sizeToFill = sizeSamps - 1;

    float phase = 0.0f;
    float delta = 1.0f / (float)sizeToFill;

    for (int i = 0; i < sizeToFill; ++i) {
        wavetable[i] = (float)sin(phase * twoPi);
        phase += delta;
    }
}

// see: https://www.musicdsp.org/en/latest/Synthesis/216-fast-whitenoise-generator.html
inline void makeWhiteNoiseWavetable(
    std::vector<float>& wavetable,
    int sizeSamps
) {
    wavetable.resize(sizeSamps, 0.0f);
    int sizeToFill = sizeSamps - 1;

    static float s_scale = 2.0f / (float)0xffffffff;
    static int s_x1 = 0x67452301;
    static int s_x2 = 0xefcdab89;

    for (int i = 0; i < sizeToFill; ++i) {
        s_x1 ^= s_x2;
        wavetable[i] = s_x2 * s_scale;
        s_x2 += s_x1;
    }
}

inline void makeTanhWavetable(
    std::vector<float>& wavetable,
    int sizeSamps,
    float mult=1.0f
) {
    wavetable.resize(sizeSamps, 0.0f);

    // change this if lerping
    // int sizeToFill = sizeSamps - 1;
    int sizeToFill = sizeSamps;

    float ratio = 1.0f / sizeToFill;
    float x = 0.0f;

    for (int i = 0; i < sizeToFill; ++i) {
        x = (((i * ratio) * 2) - 1);
        wavetable[i] = tanh(x * mult);
    }
}
