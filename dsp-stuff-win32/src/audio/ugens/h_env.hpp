#pragma once

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_constants.hpp"

struct Env {
    unsigned timer = 0;
    bool on = false;

    double get(bool trig, double holdTime, double t) {
        double out = 0.0;
        unsigned holdTimeSamps = mstosamps(holdTime);

        if (trig) {
            on = true;
            timer = 0;
        }

        if (timer < holdTimeSamps) {
            out = 1.0;
        }

        timer += 1;

        return out;
    }
};
