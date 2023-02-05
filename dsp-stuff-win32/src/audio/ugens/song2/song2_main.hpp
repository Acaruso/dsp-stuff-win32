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
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_util.hpp"

// out[0] - audio signal

namespace Song2 {

class Main : public BaseUgen {
public:
    NoteUtil noteUtil;

    Seq* seq;

    Seq* sawOpSeq;
    int sawOpCounter = 0;

    PolyWavetable* polyWt;
    AHRData longPolyWtAmpEnv{1, 100, 400};
    AHRData longPolyWtModEnv{1, 20,  300};
    AHRData shortPolyWtAmpEnv{1, 30,  100};
    AHRData shortPolyWtModEnv{1, 10,  80};

    SawOp* sawOp;

    std::vector<BaseGen*> gens;

    Notes eMaj = Notes(
        noteUtil.guitar(5, 0),
        noteUtil.guitar(4, 0),
        noteUtil.guitar(3, 1),
        noteUtil.guitar(2, 2),
        noteUtil.guitar(1, 2),
        noteUtil.guitar(0, 0)
    );

    Notes aMin = Notes(
        noteUtil.guitar(5, 0),
        noteUtil.guitar(4, 1),
        noteUtil.guitar(3, 2),
        noteUtil.guitar(2, 2),
        noteUtil.guitar(1, 0)
    );

    Notes aMinPlus = Notes(
        noteUtil.guitar(5, 3),
        noteUtil.guitar(4, 1),
        noteUtil.guitar(3, 2),
        noteUtil.guitar(2, 2),
        noteUtil.guitar(1, 0)
    );

    Notes dMaj = Notes(
        noteUtil.guitar(5, 2),
        noteUtil.guitar(4, 3),
        noteUtil.guitar(3, 2),
        noteUtil.guitar(2, 0)
    );

    Notes dMajUp = Notes(
        noteUtil.guitar(5, 2 + 3),
        noteUtil.guitar(4, 3 + 3),
        noteUtil.guitar(3, 2 + 3),
        noteUtil.guitar(2, 0)
    );

    std::vector<Notes> chordProg = { eMaj, aMin, dMaj, dMajUp, eMaj, aMinPlus, dMaj, dMajUp };

    float outSig = 0.0f;

    Env* sawFreqEnv = new Env{AHRData{1, 1, 200}};
    float sawFreq = 0.0f;

    float r = 0.0f;

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

        seq = new Seq;
        sawOpSeq = new Seq;
        polyWt = new PolyWavetable;
        sawOp = new SawOp;

        gens.push_back(seq);
        gens.push_back(sawOpSeq);
        gens.push_back(polyWt);
        gens.push_back(sawOp);
        gens.push_back(sawFreqEnv);

        polyWt->setWavetable(ugenCtx->wavetables.sin);
        polyWt->setEnv(longPolyWtAmpEnv);
        polyWt->setEnvMod(longPolyWtModEnv);
        polyWt->setModAmount(16.0f);

        // sawOp->setEnv(AHRData{200.0f, 100.0f, 300.0f});
        sawOp->setEnv(AHRData{1, 300, 300});

        sawOpSeq->setOneBarPattern(
            //                1           2           3           4
            std::vector<int>{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
            // std::vector<int>{ 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0 }
            // std::vector<int>{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0 }
        );

    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            outSig = 0.0f;

            if (seq->trigger()) {
                polyWt->setEnv(longPolyWtAmpEnv);
                polyWt->setEnvMod(longPolyWtModEnv);
                polyWt->setModAmount(16.0f);

                Freqs curFreqs = chordProg[seq->measures].toFreqs();
                polyWt->setFreqs(curFreqs);
                polyWt->trigger();
            }

            if (sawOpSeq->trigger()) {
                Notes curNotes = noteUtil.addOctaves(
                    chordProg[seq->measures]
                );

                int curNote = curNotes.elts[sawOpCounter];

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

                if (getRandBool(0.9) && (seq->_16ToM % 2) == 0) {
                    sawOp->trigger();
                }

                if (getRandBool(0.1)) {
                    sawFreqEnv->trigger();
                }

                if (getRandBool(0.2)) {
                    // Freqs curFreqs = chordProg2[
                    //     (seq->measures + 1) % seq->numMeasures
                    // ].toFreqs();
                    // polyWt->setFreqs(curFreqs);
                    polyWt->setEnv(shortPolyWtAmpEnv);
                    polyWt->setEnvMod(shortPolyWtModEnv);
                    if ((seq->_16ToM % 2) == 1) {
                        polyWt->setModAmount(22);
                    }
                    polyWt->trigger();
                    std::cout << "trig" << std::endl;
                } else {
                    std::cout << "no trig" << std::endl;
                }
                
                sawOpCounter = (sawOpCounter + 1) % curNotes.size;
            }

            if (sawFreqEnv->on) {
                outSig += polyWt->get() * 0.5f;
                // outSig += sawOp->get() * 0.12f;
                outSig += sawOp->get() * 0.24f;
                r = getRand();
                if (getRandBool(0.5)) {
                    // sawOp->setFreq(sawFreq + (sawFreqEnv->get() * (1 + (r * 2000))));
                    sawOp->setFreq(sawFreq + (sawFreqEnv->get() * 1000));
                } else {
                    sawOp->setFreq(sawFreq + (sawFreqEnv->get() * -200));
                }
            } else {
                outSig += ((polyWt->get() * 2) * (sawOp->get())) * 0.3f;
                outSig += polyWt->get() * 0.5f;
                outSig += sawOp->get() * 0.12f;
            }

            WRITE_OUT(d, out0, i, outSig);

            for (auto gen : gens) {
                gen->run();
            }
        }
    }
};

}
