#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs.hpp"
#include "src/audio/ugens/song2/song2_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// out[0] - audio signal

namespace Song2 {

class Main : public BaseUgen {
public:
    Notes notes;
    Seq seq;

    // not using
    Saw saw;
    Square square;
    Triangle triangle{1.0f};
    Wavetable wt;
    Wavetable wtMod;
    Env env{AHRData{1.0f, 200.0f, 10000.0f}};
    Env envMod{AHRData{1.0f, 20.0f, 50.0f}};

    // using
    PolyWavetable polyWt;
    SawOp sawOp;
    Seq sawOpSeq;
    int sawOpCounter = 0;

    std::vector<Freqs> chordProg2 = {
        notes.makeMajorChord(0),
        notes.makeMinorChord(9),
        notes.makeMajorChord(5),
        notes.makeMajorChord(7),
        notes.makeMajorChord(0),
        notes.makeMinorChord(9),
        notes.makeMajorChord(5),
        notes.makeMajorChord(7),
    };

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

        sawOp.setEnv(AHRData{1.0f, 50.0f, 100.0f});
        sawOpSeq.setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0 }
        );
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (seq.trigger()) {
                polyWt.setFreqs(chordProg2[seq.measures]);
                polyWt.trigger();
            }

            if (sawOpSeq.trigger()) {
                Freqs curFreqs = chordProg2[seq.measures];
                float sawOpFreq = curFreqs.f[sawOpCounter];
                sawOpCounter = (sawOpCounter + 1) % 3;

                sawOp.setFreq(sawOpFreq);
                sawOp.trigger();
            }

            outSig = (polyWt.get() * 0.5f) + (sawOp.get() * 0.15f);

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
        sawOp.run();
        sawOpSeq.run();
        env.run();
        envMod.run();
        seq.run();
    }
};

}
