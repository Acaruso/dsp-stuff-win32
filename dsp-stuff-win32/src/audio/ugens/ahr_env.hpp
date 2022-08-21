#pragma once

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_constants.hpp"

struct AHREnv {
    unsigned timer = 0;
    bool on = false;
    double sig;

    double get(bool trig, double a, double h, double r, double t) {
        unsigned attackSamps = mstosamps(a);
        unsigned holdSamps = mstosamps(h);
        unsigned releaseSamps = mstosamps(r);

        double attackDelta = 1.0 / (double)attackSamps;
        double releaseDelta = 1.0 / (double)releaseSamps;

        if (trig) {
            sig = 0.0;
            timer = 0;
            on = true;
        }

        if (timer < attackSamps) {
            sig += attackDelta;
        } else if (timer < attackSamps + holdSamps) {
            sig = 1.0;
        } else if(timer < attackSamps + holdSamps + releaseSamps) {
            sig -= releaseDelta;
        } else {
            sig = 0.0;
            on = false;
        }

        timer += 1;

        return sig;
    }
};
