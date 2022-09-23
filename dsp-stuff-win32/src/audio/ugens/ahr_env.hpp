#pragma once

#include <algorithm>
#include <iostream>

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
        ugenCtx = _ugenCtx;
        
        resizeIns(1);
        resizeOuts(2);

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
        // if (in[0][0] == 1.0f) {
        //     trigger();
        // }

        if (readIn(0, 0) == 1.0f) {
            trigger();
        }

        if (!on) {
            for (int i = 0; i < bufferSize; ++i) {
                writeOut(0, i, 0.0f);
            }
        } else {
            for (int i = 0; i < bufferSize; ++i) {
                if (timer < attackSamps) {
                    sig += attackDelta;
                } else if (timer < attackHoldSamps) {
                    sig = 1.0f;
                } else if (timer < attackHoldReleaseSamps) {
                    sig -= releaseDelta;
                } else if (timer >= attackHoldReleaseSamps) {
                    sig = 0.0f;
                    on = false;
                }

                timer += 1;

                writeOut(0, i, sig);
            }
        }

        writeOut(1, 0, on ? 1.0f : 0.0f);
    }

    void trigger() {
        on = true;
        sig = 0.0f;
        timer = 0;
    }
};
