#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// out[0] - audio signal

namespace Song2 {

class Main : public BaseUgen {
public:
    Notes notes;
    Saw saw;
    Square square;
    Triangle triangle{1.0f};
    Wavetable wt;
    Wavetable wtMod;
    PolyWavetable polyWt;
    Seq seq;
    Env env{AHRData{1.0f, 200.0f, 100.0f}};
    Env envMod{AHRData{1.0f, 20.0f, 50.0f}};

    float wtSig = 0.0f;
    float outSig = 0.0f;

    int noteCounter = 0;

    Main(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float _level=1.0f
    ) {
        typeStr = "Song2::Main";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);

        wt.setWavetable(ugenCtx->wavetables.sin);
        wtMod.setWavetable(ugenCtx->wavetables.sin);
        polyWt.setWavetable(ugenCtx->wavetables.sin);
    }

    // void run(unsigned sampleCounter) override {
    //     auto& d = ugenCtx->bufferAllocator.data;
    //     unsigned out0 = out[0];

    //     for (int i = 0; i < bufferSize; ++i) {
    //         if (seq.get()) {
    //             wt.setFreq(notes.getFreq());
    //             wtMod.setFreq(notes.getFreq() * 1.0);
    //             notes.incNote();
    //             env.trigger();
    //             envMod.trigger();
    //         }

    //         wt.setPhaseMod(wtMod.get() * envMod.get() * 8);
    //         wtSig = wt.get() * level;
    //         outSig = wtSig * env.get() * level;
    //         WRITE_OUT(d, out0, i, outSig);

    //         runAll();
    //     }
    // }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (seq.get()) {
                polyWt.setFreqs(
                    notes.getFreq(0 + noteCounter),
                    notes.getFreq(2 + noteCounter),
                    notes.getFreq(4 + noteCounter),
                    notes.getFreq(6 + noteCounter)
                );
                polyWt.trigger();
                env.trigger();
                noteCounter++;
            }

            wtSig = polyWt.get() * level;
            outSig = wtSig * env.get() * level;
            WRITE_OUT(d, out0, i, outSig);

            runAll();
        }
    }

    void runAll() {
        saw.run();
        square.run();
        triangle.run();
        wt.run();
        wtMod.run();
        polyWt.run();
        env.run();
        envMod.run();
        seq.run();
    }
};

}
