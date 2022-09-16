#pragma once

#include <algorithm>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

class AHREnv : public BaseUgen {
public:
    double a;
    double h;
    double r;

    unsigned attackSamps;
    unsigned holdSamps;
    unsigned releaseSamps;

    unsigned attackHoldSamps;
    unsigned attackHoldReleaseSamps;

    double attackDelta;
    double releaseDelta;

    bool on = false;
    double sig;
    unsigned timer = 0;

    AHREnv() {}

    AHREnv(double a_, double h_, double r_) {
        a = a_ == 0 ? 1 : a_;
        h = h_ == 0 ? 1 : h_;
        r = r_ == 0 ? 1 : r_;

        attackSamps = mstosamps(a);
        holdSamps = mstosamps(h);
        releaseSamps = mstosamps(r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0 / (double)attackSamps;
        releaseDelta = 1.0 / (double)releaseSamps;
    }

    // in[0]  - trigger
    // out[0] - envelope
    // out[1] - on/off
    
    void run(unsigned sampleCounter) override {
        std::vector<double>* out0 = getOutPtr(&out[0]);
        std::vector<double>* out1 = getOutPtr(&out[1]);

        if (in[0][0] == 1.0) {
            trigger();
        }

        if (!on) {
            std::fill((*out0).begin(), (*out0).end(), 0.0);
        } else {
            for (int i = 0; i < bufferSize; ++i) {
                if (timer < attackSamps) {
                    sig += attackDelta;
                } else if (timer < attackHoldSamps) {
                    sig = 1.0;
                } else if (timer < attackHoldReleaseSamps) {
                    sig -= releaseDelta;
                } else if (timer >= attackHoldReleaseSamps) {
                    sig = 0.0;
                    on = false;
                }

                timer += 1;

                (*out0)[i] += sig;
            }
        }


        (*out1)[0] += on ? 1.0 : 0.0;
    }

    void trigger() {
        on = true;
        sig = 0.0;
        timer = 0;
    }
};
