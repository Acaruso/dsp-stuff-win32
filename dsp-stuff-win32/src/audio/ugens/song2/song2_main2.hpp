#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_advanced_synths.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_chords.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_note_util.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/song2/song2_sidechain.hpp"
#include "src/audio/ugens/song2/song2_synths.hpp"
#include "src/audio/ugens/song2/song2_wave_player.hpp"
#include "src/audio/ugens/song2/song2_waveshaper.hpp"
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
    float submixSig = 0.0f;
    float waveshapeSig = 0.0f;

    SeqClock seqClock{200};

    AdditiveSynth additiveSynth;
    AdvancedSeq additiveSynthSeq;

    WavetableSynth wtSynth;
    Waveshaper waveshaper;
    AdvancedSeq wtSynthSeq;

    std::vector<int> wtSynthNotes = std::vector<int>{
        guitar(1, 0),
        guitar(1, 3),
        guitar(1, 5),
        guitar(2, 4),
        guitar(3, 5),
        guitar(3, 7),
        guitar(3, 4),
        guitar(2, 6),
        guitar(2, 7),
    };

    int wtSynthNotesCounter = 0;

    WavetableSynthFreqEnv kick;
    AdvancedSeq kickSeq;

    AdvSynth1 advSynth1;

    Main2(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float _level=1.0f
    ):
        additiveSynth(
            _ugenCtx->wavetables.sin,
            AHRData{0, 500, 10},
            AHRData{0, 0, 500},
            0.0f,
            0.7f
        ),
        additiveSynthSeq(
            &seqClock,
            //1           2           3           4
            { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }
        ),
        wtSynth(
            _ugenCtx->wavetables.sin,
            AHRData{0, 10, 100}
        ),
        waveshaper(
            _ugenCtx->wavetables.tanh
        ),
        wtSynthSeq(
            &seqClock,
            //1           2           3           4
            { 0, 0, 2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0 }
        ),
        kick(
            _ugenCtx->wavetables.sin,
            AHRData{0, 80, 200},
            AHRScaleData{0, 1, 130, 40, 150 }
        ),
        kickSeq(
            &seqClock,
            //1           2           3           4
            { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 }
        ),
        advSynth1(&_ugenCtx->wavetables, &seqClock)
    {
        typeStr = "Song2::Main2";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);

        additiveSynth.setFreq(180);
        wtSynth.setFreq(40);

        gens.push_back(&seqClock);
        gens.push_back(&additiveSynth);
        gens.push_back(&additiveSynthSeq);
        gens.push_back(&wtSynth);
        gens.push_back(&waveshaper);
        gens.push_back(&wtSynthSeq);
        gens.push_back(&kick);
        gens.push_back(&kickSeq);
        gens.push_back(&advSynth1);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            outSig = 0.0f;
            submixSig = 0.0f;

            if (additiveSynthSeq.trigger()) {
                int curNote = wtSynthNotes[wtSynthNotesCounter];
                wtSynthNotesCounter = (wtSynthNotesCounter + 1) % wtSynthNotes.size();
                additiveSynth.setFreq(noteToFreq(curNote + 12));
                additiveSynth.trigger();
            }

            if (kickSeq.trigger()) {
                kick.trigger();
            }

            // outSig += additiveSynth.get() * 0.2f;
            // outSig += kick.get() * 0.2f;

            outSig += advSynth1.get() * 0.3f;

            WRITE_OUT(d, out0, i, outSig);
            for (auto gen : gens) {
                gen->run();
            }
        }
    }
};

}
