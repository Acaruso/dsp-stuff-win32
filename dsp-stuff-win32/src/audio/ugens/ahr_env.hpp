#pragma once

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_constants.hpp"

struct AHREnv {
    double a;
    double h;
    double r;

    unsigned attackSamps;
    unsigned holdSamps;
    unsigned releaseSamps;

    double attackDelta;
    double releaseDelta;

    bool on = false;
    double sig;
    unsigned timer = 0;

    void trigger(double a_, double h_, double r_) {
        a = a_ == 0 ? 1 : a_;
        h = h_ == 0 ? 1 : h_;
        r = r_ == 0 ? 1 : r_;

        attackSamps = mstosamps(a);
        holdSamps = mstosamps(h);
        releaseSamps = mstosamps(r);

        attackDelta = 1.0 / (double)attackSamps;
        releaseDelta = 1.0 / (double)releaseSamps;

        on = true;
        sig = 0.0;
        timer = 0;
    }

    double get(double t) {
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
