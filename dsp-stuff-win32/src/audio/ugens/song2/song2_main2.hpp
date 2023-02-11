#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_chords.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_note_util.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_synths.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/song2/song2_wave_player.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_util.hpp"

// out[0] - audio signal

namespace Song2 {

class Main2 : public BaseUgen {
public:
    NoteUtil noteUtil;
    std::vector<BaseGen*> gens;
    float outSig = 0.0f;

    AdditiveSynth* additiveSynth = nullptr;
    SimpleSeq* additiveSynthSeq = nullptr;

    Main2(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float _level=1.0f
    ) {
        typeStr = "Song2::Main2";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);

        additiveSynth = pushGen(
            new AdditiveSynth(
                ugenCtx->wavetables.sin,
                AHRData{0, 200, 0},
                AHRData{0, 0, 200},
                0.0f,
                0.7f
            )
        );

        additiveSynth->setFreq(50);

        additiveSynthSeq = pushGen(
            new SimpleSeq(
                5000,
                //1           2           3           4
                { 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0 }
            )
        );
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            outSig = 0.0f;

            if (additiveSynthSeq->trigger()) {
                additiveSynth->trigger();
            }

            outSig += additiveSynth->get() * 0.2f;

            WRITE_OUT(d, out0, i, outSig);
            for (auto gen : gens) {
                gen->run();
            }
        }
    }

    template <typename T>
    T pushGen(T t) {
        gens.push_back(t);
        return t;
    }
};

}
