#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Saw {
public:
    float freq;
    float phase;
    float inc;

    Saw() {}

    Saw(float _freq) {
        setFreq(_freq);
    }

    // if inc == secondsPerSample, we will oscillate one time per second
    // if inc == (secondsPerSampe * freq), we will oscillate `freq` times per second
    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void run() {
        phase += inc;
        if (phase > 1.0f) {
            phase = phase - 1.0f;
        }
    }

    float get() {
        return (phase * 2.0f) - 1.0f;
    }
};

class Square {
public:
    float freq;
    float flip;
    float phase;
    float temp;
    float inc;

    Square() {}

    Square(float _freq) {
        setFreq(_freq);
    }

    // if inc == secondsPerSample, we will oscillate one time per second
    // if inc == (secondsPerSampe * freq), we will oscillate `freq` times per second
    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
        flip = 0.0f;
    }

    void setFlip(float _flip) {
        flip = _flip;
    }

    void run() {
        phase += inc;
        if (phase > 1.0f) {
            phase = phase - 1.0f;
        }
    }

    float get() {
        temp = (phase * 2.0f) - 1.0f;
        if (temp <= flip) {
            return -1.0f;
        } else {
            return 1.0f;
        }
    }
};

class Triangle {
public:
    float freq;
    float phase;
    float inc;
    float temp;

    float q0 = 0.0f;
    float q1 = 0.25f;
    float q2 = 0.5f;
    float q3 = 0.75f;

    Triangle() {}

    Triangle(float _freq) {
        setFreq(_freq);
    }

    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void run() {
        phase += inc;
        if (phase > 1.0f) {
            phase = phase - 1.0f;
        }
    }

    float get() {
        if (phase <= q1) {
            temp = phase * 4.0f;
        } else if (phase <= q3) {
            temp = (phase - 0.5f) * (-4.0f);
        } else if (phase > q3) {
            temp = (phase - 1.0f) * 4.0f;
        } else {
            temp = 0.0f;
        }

        return temp;
    }
};


}
