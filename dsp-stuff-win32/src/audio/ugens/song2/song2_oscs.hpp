#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs.hpp"
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
    float level = 1.0f;

    float freq = 0.0f;
    float phase = 0.0f;
    float phaseMod = 0.0f;

    int size = 0;
    std::vector<float>* wavetable;

    float fSize = 0.0f;
    float fSizexSecondsPerSample = 0.0f;

    int wtIdx = 0;
    float sig = 0.0f;

    Wavetable() {};

    Wavetable(
        std::vector<float>* _wavetable, 
        float _freq,
        float _level=1.0f
    ) {
        wavetable = _wavetable;
        freq = _freq;
        level = _level;
        
        size = wavetable->size() - 1;
        fSize = (float)size;
        fSizexSecondsPerSample = fSize * secondsPerSample;
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
        return sig * level;
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

class PolyWavetable {
public:
    int numOscs = 3;
    std::vector<Wavetable> oscs = std::vector<Wavetable>(numOscs);
    std::vector<Wavetable> modOscs = std::vector<Wavetable>(numOscs);
    Env envMod{AHRData{1.0f, 100.0f, 200.0f}};
    float modAmount = 4.0f;
    float sig;

    PolyWavetable() {}

    PolyWavetable(int _numOscs) {
        numOscs = _numOscs;
        oscs.resize(numOscs);
        modOscs.resize(numOscs);
    }

    PolyWavetable(AHRData ahrData) {
        envMod.setAhr(ahrData);
    }

    PolyWavetable(int _numOscs, AHRData ahrData) {
        numOscs = _numOscs;
        oscs.resize(numOscs);
        modOscs.resize(numOscs);
        envMod.setAhr(ahrData);
    }

    void setWavetable(std::vector<float>* wavetable) {
        for (int i = 0; i < numOscs; ++i) {
            oscs[i].setWavetable(wavetable);
            modOscs[i].setWavetable(wavetable);
        }
    }

    void setEnvMod(AHRData ahrData) {
        envMod.setAhr(ahrData);
    }

    void setModAmount(float _modAmount) {
        modAmount = _modAmount;
    }

    void setFreqs(Freqs freqs) {
        if (freqs.f0  != -1.0f) { oscs[0 ].setFreq(freqs.f0 ); modOscs[0 ].setFreq(freqs.f0 ); }
        if (freqs.f1  != -1.0f) { oscs[1 ].setFreq(freqs.f1 ); modOscs[1 ].setFreq(freqs.f1 ); }
        if (freqs.f2  != -1.0f) { oscs[2 ].setFreq(freqs.f2 ); modOscs[2 ].setFreq(freqs.f2 ); }
        if (freqs.f3  != -1.0f) { oscs[3 ].setFreq(freqs.f3 ); modOscs[3 ].setFreq(freqs.f3 ); }
        if (freqs.f4  != -1.0f) { oscs[4 ].setFreq(freqs.f4 ); modOscs[4 ].setFreq(freqs.f4 ); }
        if (freqs.f5  != -1.0f) { oscs[5 ].setFreq(freqs.f5 ); modOscs[5 ].setFreq(freqs.f5 ); }
        if (freqs.f6  != -1.0f) { oscs[6 ].setFreq(freqs.f6 ); modOscs[6 ].setFreq(freqs.f6 ); }
        if (freqs.f7  != -1.0f) { oscs[7 ].setFreq(freqs.f7 ); modOscs[7 ].setFreq(freqs.f7 ); }
        if (freqs.f8  != -1.0f) { oscs[8 ].setFreq(freqs.f8 ); modOscs[8 ].setFreq(freqs.f8 ); }
        if (freqs.f9  != -1.0f) { oscs[9 ].setFreq(freqs.f9 ); modOscs[9 ].setFreq(freqs.f9 ); }
        if (freqs.f10 != -1.0f) { oscs[10].setFreq(freqs.f10); modOscs[10].setFreq(freqs.f10); }
        if (freqs.f11 != -1.0f) { oscs[11].setFreq(freqs.f11); modOscs[11].setFreq(freqs.f11); }
    }

    void trigger() {
        for (int i = 0; i < numOscs; ++i) {
            oscs[i].trigger();
            modOscs[i].trigger();
        }
        envMod.trigger();
    }

    float get() {
        sig = 0.0f;
        for (int i = 0; i < numOscs; ++i) {
            sig += oscs[i].get() * 0.2;
        }
        return sig;
    }

    void run() {
        for (int i = 0; i < numOscs; ++i) {
            auto& osc = oscs[i];
            auto& modOsc = modOscs[i];
            osc.setPhaseMod(modOsc.get() * envMod.get() * modAmount);
            osc.run();
            modOsc.run();
            envMod.run();
        }
    }
};

}
