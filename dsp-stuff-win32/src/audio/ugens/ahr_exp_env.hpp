#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class AHRExpEnv : public BaseUgen {
public:
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
    float linearSig;
    float sig;
    unsigned timer = 0;

    AHRExpEnv(UgenCtx* _ugenCtx, AHRData ahrData) {
        typeStr = "AHRExpEnv";
        ugenCtx = _ugenCtx;

        numIns = 1;
        numOuts = 2;
        allocateBuffers(typeStr);

        a = ahrData.a == 0.0f ? 1 : ahrData.a;
        h = ahrData.h == 0.0f ? 1 : ahrData.h;
        r = ahrData.r == 0.0f ? 1 : ahrData.r;

        attackSamps = mstosamps(a);
        holdSamps = mstosamps(h);
        releaseSamps = mstosamps(r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        unsigned in0 = in[0];
        unsigned out0 = out[0];
        unsigned out1 = out[1];

        for (int i = 0; i < bufferSize; ++i) {
            if (READ_IN(d, in0, i) == 1.0f) {
                trigger();
            }

            if (!on) {
                WRITE_OUT(d, out0, i, 0.0f);
                WRITE_OUT(d, out1, i, 0.0f);
            } else {
                if (timer < attackSamps) {
                    linearSig += attackDelta;
                    sig = sqrt(linearSig);
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldSamps) {
                    sig = 1.0f;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldReleaseSamps) {
                    linearSig -= releaseDelta;
                    sig = linearSig * linearSig;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer >= attackHoldReleaseSamps) {
                    sig = 0.0f;
                    on = false;
                    WRITE_OUT(d, out1, i, 0.0f);
                }

                ++timer;

                WRITE_OUT(d, out0, i, sig * level);
            }
        }
    }

    inline void trigger() {
        on = true;
        sig = 0.0f;
        timer = 0;
    }
};
