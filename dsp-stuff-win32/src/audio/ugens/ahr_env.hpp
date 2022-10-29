#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class AHREnv : public BaseUgen {
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
    float sig;
    unsigned timer = 0;

    AHREnv(UgenCtx* _ugenCtx, float a_, float h_, float r_) {
        typeStr = "AHREnv";
        ugenCtx = _ugenCtx;

        numIns = 1;
        numOuts = 2;
        allocateBuffers(typeStr);

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
                    sig += attackDelta;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldSamps) {
                    sig = 1.0f;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldReleaseSamps) {
                    sig -= releaseDelta;
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
