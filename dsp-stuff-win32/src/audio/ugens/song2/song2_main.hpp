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
    SawOp* sawOp;

    std::vector<BaseGen*> gens;

    // std::vector<Notes> chordProg2 = {
    //     noteUtil.makeMajorChord(0),
    //     noteUtil.makeMinorChord(9),
    //     noteUtil.makeMajorChord(5),
    //     noteUtil.makeMajorChord(7),
    //     noteUtil.makeMajorChord(0),
    //     noteUtil.makeMinorChord(9),
    //     noteUtil.makeMajorChord(5),
    //     noteUtil.makeMajorChord(7),
    // };

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

    std::vector<Notes> chordProg2 = { eMaj, aMin, dMaj, dMajUp, eMaj, aMinPlus, dMaj, dMajUp };

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

        seq = new Seq;
        sawOpSeq = new Seq;
        polyWt = new PolyWavetable;
        sawOp = new SawOp;

        gens.push_back(seq);
        gens.push_back(sawOpSeq);
        gens.push_back(polyWt);
        gens.push_back(sawOp);

        polyWt->setWavetable(ugenCtx->wavetables.sin);
        polyWt->setEnv(AHRData{1.0f, 100.0f, 600.0f});
        polyWt->setEnvMod(AHRData{1.0f, 20.0f, 500.0f});
        polyWt->setModAmount(16.0f);

        sawOp->setEnv(AHRData{1.0f, 50.0f, 100.0f});

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
                Freqs curFreqs = chordProg2[seq->measures].toFreqs();
                polyWt->setFreqs(curFreqs);
                polyWt->trigger();
            }

            if (sawOpSeq->trigger()) {
                Notes curNotes = noteUtil.addOctaves(
                    chordProg2[seq->measures]
                );

                int curNote = curNotes.elts[sawOpCounter];

                float r = getRand();

                if (r < 0.4) {
                    curNote += 12;
                } else if (r > 0.7) {
                    curNote += 7;
                }

                float curFreq = noteToFreq(curNote);

                sawOp->setFreq(curFreq);
                sawOp->trigger();

                sawOpCounter = (sawOpCounter + 1) % curNotes.size;
            }

            outSig += polyWt->get() * 0.5f;
            outSig += sawOp->get() * 0.15f;

            WRITE_OUT(d, out0, i, outSig);

            for (auto gen : gens) {
                gen->run();
            }
        }
    }
};

}
