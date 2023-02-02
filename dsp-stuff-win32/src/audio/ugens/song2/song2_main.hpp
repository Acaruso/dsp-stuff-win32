#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs.hpp"
#include "src/audio/ugens/song2/song2_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// out[0] - audio signal

namespace Song2 {

class Main : public BaseUgen {
public:
    Notes notes;
    Seq seq;

    Saw saw;
    Square square;
    Triangle triangle{1.0f};
    Wavetable wt;
    Wavetable wtMod;
    PolyWavetable polyWt;

    Env env{AHRData{1.0f, 200.0f, 10000.0f}};
    Env envMod{AHRData{1.0f, 20.0f, 50.0f}};

    std::vector<int> chordProg = { 0, 5, 3, 4, 0, 5, 3, 4, };
    int chordProgIdx = 0;

    float wtSig = 0.0f;
    float outSig = 0.0f;

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
        polyWt.setEnv(AHRData{1.0f, 100.0f, 600.0f});
        polyWt.setEnvMod(AHRData{1.0f, 20.0f, 500.0f});
        polyWt.setModAmount(16.0f);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (seq.trigger()) {
                polyWt.setFreqs(
                    notes.makeChord(chordProg[seq.measures])
                );
                polyWt.trigger();
                env.trigger();
            }

            wtSig = polyWt.get() * level;
            outSig = wtSig * level * 0.5;
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
