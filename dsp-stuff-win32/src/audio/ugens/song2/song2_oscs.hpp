#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
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

class SawOp : public BaseGen {
public:
    float freq;
    float phase;
    float inc;
    Env env;

    SawOp() {}

    SawOp(float _freq) {
        setFreq(_freq);
    }

    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void setEnv(AHRData ahrData) {
        env.setAhr(ahrData);
    }

    void trigger() {
        phase = 0.0f;
        env.trigger();
    }

    float get() {
        return ((phase * 2.0f) - 1.0f) * env.get();
    }

    void run() override {
        phase += inc;
        if (phase > 1.0f) {
            phase = phase - 1.0f;
        }
        env.run();
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

class PolyWavetable : public BaseGen {
public:
    int numOscs = 4;
    std::vector<Wavetable> oscs = std::vector<Wavetable>(numOscs);
    std::vector<Wavetable> modOscs = std::vector<Wavetable>(numOscs);
    std::vector<bool> oscOn = std::vector<bool>(numOscs, false);

    Env env;
    Env envMod;
    float modAmount;
    float sig;

    PolyWavetable() {}

    void setNumOscs(int _numOscs) {
        numOscs = _numOscs;
        oscs.resize(numOscs);
        modOscs.resize(numOscs);
        oscOn.resize(numOscs, false);
    }

    void setWavetable(std::vector<float>* wavetable) {
        for (int i = 0; i < numOscs; ++i) {
            oscs[i].setWavetable(wavetable);
            modOscs[i].setWavetable(wavetable);
        }
    }

    void setEnv(AHRData ahrData) {
        env.setAhr(ahrData);
    }

    void setEnvMod(AHRData ahrData) {
        envMod.setAhr(ahrData);
    }

    void setModAmount(float _modAmount) {
        modAmount = _modAmount;
    }

    void setFreqs(Freqs freqs) {
        std::fill(oscOn.begin(), oscOn.end(), false);

        for (int i = 0; i < freqs.size; ++i) {
            oscs[i].setFreq(freqs.f[i]);
            modOscs[i].setFreq(freqs.f[i]);
            oscOn[i] = true;
        }
    }
    
    void trigger() {
        bool anyOscsOn = false;

        for (int i = 0; i < numOscs; ++i) {
            if (oscOn[i]) {
                anyOscsOn = true;
                oscs[i].trigger();
                modOscs[i].trigger();
            }
        }

        if (anyOscsOn) {
            env.trigger();
            envMod.trigger();
        }
    }

    float get() {
        sig = 0.0f;
        for (int i = 0; i < numOscs; ++i) {
            if (oscOn[i]) {
                sig += oscs[i].get() * env.get() * 0.2;
            }
        }
        return sig;
    }

    void run() override {
        for (int i = 0; i < numOscs; ++i) {
            auto& osc = oscs[i];
            auto& modOsc = modOscs[i];
            osc.setPhaseMod(modOsc.get() * envMod.get() * modAmount);
            osc.run();
            modOsc.run();
        }
        env.run();
        envMod.run();
    }
};

}
