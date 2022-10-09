#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class AHRExpEnv : public BaseUgen {
public:
    int wtSize = 1024;
    float ratio;
    int wtIdx;
    float f_wtIdx;
    std::vector<float> wavetable = std::vector<float>(wtSize, 0.0f);

    float a;
    float h;
    float r;

    unsigned attackSamps;
    unsigned holdSamps;
    unsigned releaseSamps;

    unsigned attackHoldSamps;
    unsigned attackHoldReleaseSamps;

    float attackDelta;
    float releaseDelta;

    bool on = false;
    float sig;
    unsigned timer = 0;

    AHRExpEnv(UgenCtx* _ugenCtx, float a_, float h_, float r_) {
        ugenCtx = _ugenCtx;

        numIns = 1;
        numOuts = 2;
        allocateBuffers("AHRExpEnv");

        a = a_ == 0.0f ? 1 : a_;
        h = h_ == 0.0f ? 1 : h_;
        r = r_ == 0.0f ? 1 : r_;

        attackSamps = mstosamps(a);
        holdSamps = mstosamps(h);
        releaseSamps = mstosamps(r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;

        fillWavetable();
    }

    void fillWavetable() {
        int wtSizeToFill = wtSize - 1;

        ratio = (float)wtSize / (float)attackHoldReleaseSamps;

        int attackTimeWt = wtSizeToFill * ((float)attackSamps / (float)attackHoldReleaseSamps);
        int holdTimeWt = wtSizeToFill * ((float)holdSamps / (float)attackHoldReleaseSamps);
        int releaseTimeWt = wtSizeToFill * ((float)releaseSamps / (float)attackHoldReleaseSamps);

        float attackDeltaWt = 1.0f / (float)attackTimeWt;
        float releaseDeltaWt = 1.0f / (float)releaseTimeWt;

        float linearSig = 0.0f;
        float sig = 0.0f;

        for (int i = 0; i < wtSizeToFill; i++) {
            wavetable[i] = sig;

            if (i < attackTimeWt) {
                linearSig += attackDeltaWt;
                sig = sqrt(linearSig);
            } else if (i < attackTimeWt + holdTimeWt) {
                sig = 1.0f;
            } else if (i < attackTimeWt + holdTimeWt + releaseTimeWt) {
                linearSig -= releaseDeltaWt;
                sig = linearSig * linearSig;
            }
        }

        wavetable[wtSize - 1] = 0.0f;
    }

    // no interpolation

    // void run(unsigned sampleCounter) override {
    //     auto& d = ugenCtx->bufferAllocator.data;
    //     unsigned in0 = in[0];
    //     unsigned out0 = out[0];
    //     unsigned out1 = out[1];

    //     if (READ_IN(d, in0, 0) == 1.0f) {
    //         trigger();
    //     }

    //     if (!on) {
    //         fillBuffer(d, out0, bufferSize, 0.0f);
    //     } else {
    //         for (int i = 0; i < bufferSize; ++i) {
    //             if (timer < attackHoldReleaseSamps) {
    //                 wtIdx = timer * ratio;
    //                 sig = wavetable[wtIdx];
    //             } else {
    //                 sig = 0.0f;
    //                 on = false;
    //             }

    //             ++timer;

    //             WRITE_OUT(d, out0, i, sig);
    //         }
    //     }

    //     WRITE_OUT(d, out1, 0, on ? 1.0f : 0.0f);
    // }

    // linear interpolation

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned out0 = out[0];
        unsigned out1 = out[1];

        if (READ_IN(d, in0, 0) == 1.0f) {
            trigger();
        }

        if (!on) {
            fillBuffer(d, out0, bufferSize, 0.0f);
        } else {
            for (int i = 0; i < bufferSize; ++i) {
                if (timer < attackHoldReleaseSamps) {
                    f_wtIdx = timer * ratio;
                    wtIdx = (int)f_wtIdx;
                    sig = LERP_WT(wavetable, wtIdx, f_wtIdx);
                } else {
                    sig = 0.0f;
                    on = false;
                }

                ++timer;

                WRITE_OUT(d, out0, i, sig);
            }
        }

        WRITE_OUT(d, out1, 0, on ? 1.0f : 0.0f);
    }

    inline void trigger() {
        on = true;
        sig = 0.0f;
        timer = 0;
    }
};
