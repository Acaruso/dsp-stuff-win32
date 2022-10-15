#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_constants.hpp"

inline void makeAHRWavetable(
    std::vector<float>& wavetable,
    int sizeSamps,     // desired size of wavetable in samples
    float attackMs,
    float holdMs,
    float releaseMs
) {
    wavetable.resize(sizeSamps, 0.0f);

    int attackSamps = mstosamps(attackMs);
    int holdSamps = mstosamps(holdMs);
    int releaseSamps = mstosamps(releaseMs);

    int audioSizeSamps = attackSamps + holdSamps + releaseSamps;

    int sizeToFillSamps = sizeSamps - 1;

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

        wavetable[i] = sig;
    }
}

inline void makeSinWavetable(
    std::vector<float>& wavetable,
    int size
) {
    wavetable.resize(size, 0.0f);

    int sizeToFill = size - 1;

    float phase = 0.0f;
    float delta = 1.0f / (float)sizeToFill;

    for (int i = 0; i < sizeToFill; ++i) {
        wavetable[i] = (float)sin(phase * twoPi);
        phase += delta;
    }
}
