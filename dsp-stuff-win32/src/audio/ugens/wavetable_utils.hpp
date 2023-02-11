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

    int attackSamps = mstosampsFloor1(ahrData.a);
    int holdSamps = mstosampsFloor1(ahrData.h);
    int releaseSamps = mstosampsFloor1(ahrData.r);

    int audioSizeSamps = attackSamps + holdSamps + releaseSamps;

    // float audioToWtRatio = (float)sizeToFillSamps / (float)audioSizeSamps;
    float audioToWtRatio = 0.0f;

    if (audioSizeSamps <= sizeToFillSamps) {
        audioToWtRatio = 1.0f;
    } else {
        audioToWtRatio = (float)sizeToFillSamps / (float)audioSizeSamps;
    }

    int attackSampsWt = attackSamps * audioToWtRatio;
    attackSampsWt = attackSampsWt > 0 ? attackSampsWt : 1;

    int holdSampsWt = holdSamps * audioToWtRatio;
    holdSampsWt = holdSampsWt > 0 ? holdSampsWt : 1;

    int releaseSampsWt = releaseSamps * audioToWtRatio;
    releaseSampsWt = releaseSampsWt > 0 ? releaseSampsWt : 1;

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

inline std::vector<float>* makeSinWavetable(int sizeSamps) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    int sizeToFill = sizeSamps - 1;

    float phase = 0.0f;
    float delta = 1.0f / (float)sizeToFill;

    for (int i = 0; i < sizeToFill; ++i) {
        (*wavetable)[i] = (float)sin(phase * twoPi);
        phase += delta;
    }

    return wavetable;
}

// see: https://www.musicdsp.org/en/latest/Synthesis/216-fast-whitenoise-generator.html
inline std::vector<float>* makeWhiteNoiseWavetable(int sizeSamps) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    int sizeToFill = sizeSamps - 1;

    static float s_scale = 2.0f / (float)0xffffffff;
    static int s_x1 = 0x67452301;
    static int s_x2 = 0xefcdab89;

    for (int i = 0; i < sizeToFill; ++i) {
        s_x1 ^= s_x2;
        (*wavetable)[i] = s_x2 * s_scale;
        s_x2 += s_x1;
    }

    return wavetable;
}

inline std::vector<float>* makeTanhWavetable(int sizeSamps, float mult=1.0f) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    // currently, waveshaper doesn't lerp
    // need to change this if lerping:
    // int sizeToFill = sizeSamps - 1;
    int sizeToFill = sizeSamps;

    float ratio = 1.0f / sizeToFill;
    float x = 0.0f;

    for (int i = 0; i < sizeToFill; ++i) {
        x = (((i * ratio) * 2) - 1);
        (*wavetable)[i] = tanh(x * mult);
    }

    return wavetable;
}

inline std::vector<float>* makeSawWavetable(int sizeSamps) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    int sizeToFill = sizeSamps;

    float inc = 2.0f / sizeToFill;

    float sig = -1;

    for (int i = 0; i < sizeToFill; ++i) {
        (*wavetable)[i] = sig;
        sig += inc;
    }

    return wavetable;
}

inline std::vector<float>* makeSquareWavetable(int sizeSamps) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    int halfwaySample = wavetable->size() / 2;

    for (int i = 0; i < wavetable->size(); ++i) {
        if (i < halfwaySample) {
            (*wavetable)[i] = -1.0f;
        } else {
            (*wavetable)[i] = 1.0f;
        }
    }

    return wavetable;
}

inline std::vector<float>* makeTriangleWavetable(int sizeSamps) {
    std::vector<float>* wavetable = new std::vector<float>(sizeSamps, 0.0f);

    float inc = 1.0f / wavetable->size();
    float phase = 0;
    float sig = 0;

    int q1 = 0.25f * wavetable->size();
    int q3 = 0.75f * wavetable->size();

    for (int i = 0; i < wavetable->size(); ++i) {
        if (i < q1) {
            sig = phase * 4;
        } else if (i < q3) {
            sig = (phase - 0.5f) * -4.0f;
        } else if (i >= q3) {
            sig = (phase - 1) * 4;
        } else { 
            sig = 0;
        }

        (*wavetable)[i] = sig;
        
        phase += inc;
    }

    return wavetable;
}

inline std::vector<float>* makeBitcrushWavetable(int _sizeSamps, int _numBits) {
    std::vector<float>* wavetable = new std::vector<float>(_sizeSamps, 0.0f);

    float inc = 2.0f / wavetable->size();
    float phase = -1;

    float sig = 0;
    int numBits = _numBits;
    int stepLen = wavetable->size() / numBits;
    
    for (int i = 0; i < wavetable->size(); ++i) {
        if (i % stepLen == 0) {
            sig = phase;
        }

        (*wavetable)[i] = sig;

        phase += inc;
    }

    return wavetable;
}
