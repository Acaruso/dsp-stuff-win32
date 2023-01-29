#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Saw {
public:
    float freq;
    float samp;
    float inc;

    Saw() {}

    Saw(float _freq) {
        freq = _freq;
    }

    // if inc == secondsPerSample, we will oscillate one time per second
    // if inc == (secondsPerSampe * freq), we will oscillate `freq` times per second
    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void run() {
        samp += inc;
        if (samp > 1.0f) {
            samp = samp - 1.0f;
        }
    }

    float get() {
        return (samp * 2.0f) - 1.0f;
    }
};

}
