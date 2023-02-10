#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_note_util.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/song2/song2_wave_player.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_util.hpp"

#include "src/audio/ugens/song2/song2_chords.hpp"

// out[0] - audio signal

namespace Song2 {

template <typename T>
inline T pushGen(std::vector<BaseGen*>& gens, T t) {
    gens.push_back(t);
    return t;
}

class Main : public BaseUgen {
public:
    NoteUtil noteUtil;

    Seq* seq = nullptr;

    SimpleSeq* sawOpSeq = nullptr;
    int sawOpCounter = 0;

    PolyWavetable* polyWt = nullptr;

    AHRData longPolyWtAmpEnv{1, 100, 400};
    AHRData longPolyWtModEnv{1, 20,  300};
    AHRData shortPolyWtAmpEnv{1, 30,  100};
    AHRData shortPolyWtModEnv{1, 10,  80};

    SawOp* sawOp;

    WavetableOpFreqEnv* kick = nullptr;
    SimpleSeq* kickSeq = nullptr;

    WavePlayer* snare = nullptr;
    SimpleSeq* snareSeq = nullptr;

    WavePlayer* hiHat = nullptr;
    SimpleSeq* hiHatSeq = nullptr;

    std::vector<BaseGen*> gens;

    std::vector<std::vector<Notes>> chordProgs = {
        { eMajPlus4, cMaj, cMajPlus, cMajUp, dMaj, cMaj,     cMajPlus, cMajUp },
        { eMajPlus4, cMaj, cMajPlus, cMajUp, dMaj, cMaj,     cMajPlus, cMajUp },
        { eMaj, aMin, dMaj,     dMajUp, eMaj, aMinPlus, dMaj,     dMajUp },
        { eMaj, aMin, dMaj,     dMajUp, eMaj, aMinPlus, dMaj,     dMajUp },
    };

    float submix = 0.0f;
    float outSig = 0.0f;

    // Env* sawFreqEnv = new Env{AHRData{1, 1, 200}};
    Env* sawFreqEnv = nullptr;

    float sawFreq = 0.0f;

    float r = 0.0f;
    bool rb = false;
    bool square = false;

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

        seq        = pushGen(gens, new Seq(5000));
        sawOpSeq   = pushGen(gens, new SimpleSeq(5000));
        polyWt     = pushGen(gens, new PolyWavetable);
        sawOp      = pushGen(gens, new SawOp);
        sawFreqEnv = pushGen(gens, new Env{AHRData{1, 1, 200}});

        kick = pushGen(
            gens, 
            new WavetableOpFreqEnv{
                ugenCtx->wavetables.sin,
                AHRData{1, 80, 100},
                AHRData{1, 1, 130},
                35,
                280
            }
        );

        kickSeq  = pushGen(gens, new SimpleSeq);
        snare    = pushGen(gens, new WavePlayer(&(ugenCtx->waves.snare1)));
        snareSeq = pushGen(gens, new SimpleSeq);
        hiHat    = pushGen(gens, new WavePlayer(&(ugenCtx->waves.hiHat1)));
        hiHatSeq = pushGen(gens, new SimpleSeq);
        
        polyWt->setWavetable(ugenCtx->wavetables.sin);
        polyWt->setEnv(longPolyWtAmpEnv);
        polyWt->setEnvMod(longPolyWtModEnv);
        polyWt->setModAmount(16.0f);

        sawOp->setEnv(AHRData{1, 80, 1});

        seq->setChordProgs(chordProgs);

        sawOpSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
        );

        kickSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }
        );

        kickSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }
        );

        snareSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }
        );

        hiHatSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }
        );
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            submix = 0.0f;
            outSig = 0.0f;

            if (seq->trigger()) {
                polyWt->setEnv(longPolyWtAmpEnv);
                polyWt->setEnvMod(longPolyWtModEnv);
                polyWt->setModAmount(16.0f);

                Freqs curFreqs = seq->getCurChord().toFreqs();

                polyWt->setFreqs(curFreqs);
                polyWt->trigger();

                if (getRandBool(0.2)) {
                    polyWt->mult = true;
                    rb = getRandBool(0.4);
                } else {
                    polyWt->mult = false;
                    rb = false;
                }
            }

            if (sawOpSeq->trigger()) {
                Notes curNotes;
                if (getRandBool(0.6)) {
                    curNotes = noteUtil.addOctaves(seq->getCurChord());
                } else {
                    curNotes = seq->getCurChord().transpose(12);
                }

                int curNote = curNotes.elts[
                    sawOpCounter < curNotes.size ? sawOpCounter : curNotes.size - 1
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

                sawOp->setFreq(sawFreq);

                sawOp->trigger();

                if (getRandBool(0.1)) {
                    sawFreqEnv->trigger();
                }

                if (getRandBool(0.2)) {
                    polyWt->setEnv(shortPolyWtAmpEnv);
                    polyWt->setEnvMod(shortPolyWtModEnv);
                    polyWt->trigger();
                    if ((seq->_16ToM % 2) == 1) {
                        polyWt->setModAmount(22);
                    }
                }
                
                sawOpCounter = (sawOpCounter + 1) % curNotes.size;

                if (polyWt->mult) {
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
                submix += polyWt->get() * 0.5f;
                submix += sawOp->get() * 0.18f;
                sawOp->setFreq(sawFreq + (sawFreqEnv->get() * 1000));
            } else {
                submix += polyWt->get() * 0.5f;
                if (!polyWt->mult || rb) {
                    outSig += sawOp->get() * 0.09f;
                }
            }

            submix += ((0.1 + polyWt->get()) * sawOp->get() * sawOp->get() * 0.3f);

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
};

}
