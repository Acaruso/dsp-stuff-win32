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

    PolyWavetableSynth* polyWtSynth = nullptr;

    Seq* polyWtSynthSeq = nullptr;

    std::vector<std::vector<Notes>> polyWtSynthChordProgs = {
        { eMajPlus4, cMaj, cMajPlus, cMajUp, dMaj, cMaj,     cMajPlus, cMajUp },
        { eMajPlus4, cMaj, cMajPlus, cMajUp, dMaj, cMaj,     cMajPlus, cMajUp },
        { eMaj, aMin, dMaj,     dMajUp, eMaj, aMinPlus, dMaj,     dMajUp },
        { eMaj, aMin, dMaj,     dMajUp, eMaj, aMinPlus, dMaj,     dMajUp },
    };

    AHRData longPolyWtAmpEnv{1, 100, 400};
    AHRData longPolyWtModEnv{1, 20,  300};
    AHRData shortPolyWtAmpEnv{1, 30,  100};
    AHRData shortPolyWtModEnv{1, 10,  80};

    SawSynth* sawSynth;
    Env* sawFreqEnv = nullptr;
    SimpleSeq* sawSynthSeq = nullptr;
    float sawFreq = 0.0f;
    int sawSynthCounter = 0;

    WavetableSynthFreqEnv* kick = nullptr;
    SimpleSeq* kickSeq = nullptr;

    WavePlayer* snare = nullptr;
    SimpleSeq* snareSeq = nullptr;

    WavePlayer* hiHat = nullptr;
    SimpleSeq* hiHatSeq = nullptr;

    float submix = 0.0f;
    float outSig = 0.0f;

    float r = 0.0f;
    bool rb = false;
    bool square = false;

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

        polyWtSynth = pushGen(
            new PolyWavetableSynth(
                ugenCtx->wavetables.sin,
                longPolyWtAmpEnv,
                longPolyWtModEnv,
                16
            )
        );

        polyWtSynthSeq = pushGen(
            new Seq(
                5000,
                //1           2           3           4
                { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
                polyWtSynthChordProgs
            )
        );

        sawSynth = pushGen(
            new SawSynth(
                AHRData{1, 80, 1}
            )
        );

        sawFreqEnv = pushGen(
            new Env(
                AHRData{1, 1, 200}
            )
        );

        sawSynthSeq = pushGen(
            new SimpleSeq(
                5000,
                //1           2           3           4
                { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
            )
        );

        kick = pushGen(
            new WavetableSynthFreqEnv{
                ugenCtx->wavetables.sin,
                AHRData{1, 80, 100},
                AHRData{1, 1, 130},
                35,
                280
            }
        );

        kickSeq = pushGen(
            new SimpleSeq(
                5000,
                //1           2           3           4
                { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }
            )
        );

        snare = pushGen(
            new WavePlayer(
                &(ugenCtx->waves.snare1)
            )
        );

        snareSeq = pushGen(
            new SimpleSeq(
                5000,
                //1           2           3           4
                { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }
            )
        );

        hiHat = pushGen(
            new WavePlayer(
                &(ugenCtx->waves.hiHat1)
            )
        );

        hiHatSeq = pushGen(
            new SimpleSeq(
                5000,
                //1           2           3           4
                { 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }
            )
        );
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            submix = 0.0f;
            outSig = 0.0f;

            if (polyWtSynthSeq->trigger()) {
                polyWtSynth->setEnv(longPolyWtAmpEnv);
                polyWtSynth->setEnvMod(longPolyWtModEnv);
                polyWtSynth->setModAmount(16.0f);

                Freqs curFreqs = polyWtSynthSeq->getCurChord().toFreqs();

                polyWtSynth->setFreqs(curFreqs);
                polyWtSynth->trigger();

                if (getRandBool(0.2)) {
                    polyWtSynth->mult = true;
                    rb = getRandBool(0.4);
                } else {
                    polyWtSynth->mult = false;
                    rb = false;
                }
            }

            if (sawSynthSeq->trigger()) {
                Notes curNotes;
                if (getRandBool(0.6)) {
                    curNotes = noteUtil.addOctaves(polyWtSynthSeq->getCurChord());
                } else {
                    curNotes = polyWtSynthSeq->getCurChord().transpose(12);
                }

                int curNote = curNotes.elts[
                    sawSynthCounter < curNotes.size ? sawSynthCounter : curNotes.size - 1
                ];

                float r = getRand();
                if (r < 0.4) {
                    curNote += 12;
                } else if (r > 0.7) {
                    curNote += 7;
                }

                if (getRandBool(0.3)) {
                    curNote -= 12;
                }

                sawFreq = noteToFreq(curNote);

                sawSynth->setFreq(sawFreq);

                sawSynth->trigger();

                if (getRandBool(0.1)) {
                    sawFreqEnv->trigger();
                }

                if (getRandBool(0.2)) {
                    polyWtSynth->setEnv(shortPolyWtAmpEnv);
                    polyWtSynth->setEnvMod(shortPolyWtModEnv);
                    polyWtSynth->trigger();
                    if ((polyWtSynthSeq->_16ToM % 2) == 1) {
                        polyWtSynth->setModAmount(22);
                    }
                }
                
                sawSynthCounter = (sawSynthCounter + 1) % curNotes.size;

                if (polyWtSynth->mult) {
                    if (getRandBool(0.7)) {
                        kick->mult = true;
                        kick->trigger();
                    }
                } else {
                    kick->mult = false;
                }
            }

            if (kickSeq->trigger()) {
                kick->trigger();
            }

            if (snareSeq->trigger()) {
                snare->trigger();
            }

            if (hiHatSeq->trigger()) {
                hiHat->trigger();
            }

            if (sawFreqEnv->on) {
                submix += polyWtSynth->get() * 0.5f;
                submix += sawSynth->get() * 0.18f;
                sawSynth->setFreq(sawFreq + (sawFreqEnv->get() * 1000));
            } else {
                submix += polyWtSynth->get() * 0.5f;
                if (!polyWtSynth->mult || rb) {
                    outSig += sawSynth->get() * 0.09f;
                }
            }

            submix += ((0.1 + polyWtSynth->get()) * sawSynth->get() * sawSynth->get() * 0.3f);

            if (kick->mult) {
                submix = submix * ((toSquare(kick->get()) * 0.7) + 0.3);
            }

            outSig += kick->get() * 0.34 + (submix * (1.0 + (-kick->ampEnv.get() * 0.5)));

            outSig += snare->get() * 0.2f;

            outSig += hiHat->get() * 0.07f;

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
