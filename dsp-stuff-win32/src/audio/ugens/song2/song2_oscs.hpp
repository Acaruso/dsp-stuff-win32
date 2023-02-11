#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
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

class Wavetable {
public:
    float freq = 0.0f;
    float phase = 0.0f;
    float phaseMod = 0.0f;

    std::vector<float>* wavetable;
    int size = 0;
    float fSize = 0.0f;
    float fSizexSecondsPerSample = 0.0f;

    int wtIdx = 0;
    float sig = 0.0f;

    Wavetable() {};

    Wavetable(
        std::vector<float>* _wavetable, 
        float _freq
    ) {
        setWavetable(_wavetable);
        freq = _freq;
        setFreq(_freq);
    }

    void setWavetable(std::vector<float>* _wavetable) {
        wavetable = _wavetable;
        size = wavetable->size() - 1;
        fSize = (float)size;
        fSizexSecondsPerSample = fSize * secondsPerSample;
    }

    void setFreq(float _freq) {
        freq = _freq;
    }

    void setPhaseMod(float _phaseMod) {
        phaseMod = _phaseMod;
    }

    void trigger() {
        phase = 0.0f;
    }

    float get() {
        return sig;
    }

    void run() {
        wtIdx = (int)phase;

        sig = LERP_WT((*wavetable), wtIdx, phase);

        // get next phase
        phase += (fSizexSecondsPerSample * freq) + phaseMod;

        // phase = phase % wavetable size
        while (phase >= fSize) {
            phase -= fSize;
        }

        while (phase < 0) {
            phase += fSize;
        }
    }
};

}
