#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class SawSynth : public BaseGen {
public:
    float freq;
    float phase;
    float inc;
    Env env;

    SawSynth() {}

    SawSynth(float _freq) {
        setFreq(_freq);
    }

    SawSynth(AHRData _ahrData) {
        setEnv(_ahrData);
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
        // phase += inc;
        phase += (secondsPerSample * freq);
        if (phase > 1.0f) {
            phase = phase - 1.0f;
        }
        env.run();
    }
};

class WavetableSynthFreqEnv : public BaseGen {
public:
    Wavetable wavetable;
    Env ampEnv;
    Env freqEnv;
    float modAmount = 0.0f;
    bool mult = false;
    int multCounter = 0;
    int multCounter2 = 0;

    WavetableSynthFreqEnv() {}

    WavetableSynthFreqEnv(
        std::vector<float>* _wavetable, 
        AHRData ampEnv, 
        AHRData freqEnv,
        float low,
        float high
    ) {
        setWavetable(_wavetable);
        setAmpEnv(ampEnv);
        setFreqEnv(freqEnv);
        setFreqEnvLow(low);
        setFreqEnvHigh(high);
    }

    void setWavetable(std::vector<float>* _wavetable) {
        wavetable.setWavetable(_wavetable);
    }

    void setAmpEnv(AHRData ahrData) {
        ampEnv.setAhr(ahrData);
    }

    void setFreqEnv(AHRData ahrData) {
        freqEnv.setAhr(ahrData);
    }

    void setFreqEnvLow(float f) {
        freqEnv.setLow(f);
    }

    void setFreqEnvHigh(float f) {
        freqEnv.setHigh(f);
    }

    void setModAmount(float f) {
        modAmount = f;
    }

    void trigger() {
        wavetable.trigger();
        ampEnv.trigger();
        freqEnv.trigger();
        if (mult) {
            ++multCounter;
            if (multCounter == 2) multCounter = 0;
            ++multCounter2;
            if (multCounter2 == 4) multCounter2 = 0;
        }
    }

    float get() {
        if (mult && multCounter == 0) {
            // return (wavetable.get() * ampEnv.get()) * (wavetable.get());
            return (
                (wavetable.get() * ampEnv.get()) * toSquare(wavetable.get())
            );
        } else {
            return wavetable.get() * ampEnv.get();
        }
    }

    void run() override {
        if (mult && multCounter == 0) {
            // wavetable.setFreq(freqEnv.get() * 4);
            wavetable.setFreq(freqEnv.get() * (multCounter2 + 2));
        } else {
            wavetable.setFreq(freqEnv.get());
        }
        wavetable.run();
        ampEnv.run();
        freqEnv.run();
    }
};

class PolyWavetableSynth : public BaseGen {
public:
    int numOscs = 6;
    std::vector<Wavetable> oscs = std::vector<Wavetable>(numOscs);
    std::vector<Wavetable> modOscs = std::vector<Wavetable>(numOscs);
    std::vector<bool> oscOn = std::vector<bool>(numOscs, false);

    Env env;
    Env envMod;
    float modAmount;
    float sig;
    bool mult = false;

    PolyWavetableSynth() {}

    PolyWavetableSynth(std::vector<float>* _wavetable) {
        setWavetable(_wavetable);
    }

    PolyWavetableSynth(
        std::vector<float>* _wavetable, 
        AHRData _ampEnvData, 
        AHRData _modEnvData,
        float _modAmount
    ) {
        setWavetable(_wavetable);
        setEnv(_ampEnvData);
        setEnvMod(_modEnvData);
        setModAmount(_modAmount);
    }

    void setNumOscs(int _numOscs) {
        numOscs = _numOscs;
        oscs.resize(numOscs);
        modOscs.resize(numOscs);
        oscOn.resize(numOscs, false);
    }

    void setWavetable(std::vector<float>* _wavetable) {
        for (int i = 0; i < numOscs; ++i) {
            oscs[i].setWavetable(_wavetable);
            modOscs[i].setWavetable(_wavetable);
        }
    }

    void setEnv(AHRData _ahrData) {
        env.setAhr(_ahrData);
    }

    void setEnvMod(AHRData _ahrData) {
        envMod.setAhr(_ahrData);
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
                if (mult) {
                    sig += (oscs[i].get() * oscs[i].get()) * env.get() * 0.2;
                } else {
                    sig += oscs[i].get() * env.get() * 0.2;
                }
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
