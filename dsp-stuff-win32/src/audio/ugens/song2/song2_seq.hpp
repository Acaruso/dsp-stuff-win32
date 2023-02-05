#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Seq : public BaseGen {
public:
    int samplesPer16thNote;

    int sTo16 = 0;
    bool sTo16Rollover = false;

    // 16th note counter
    int _16ToM = 0;
    bool _16ToMRollover = false;

    // measure counter
    int measures = 0;
    bool measuresRollover = false;

    int chordProgCounter = 0;
    bool chordProgCounterRollover = false;

    std::vector<int> oneBarPattern;
    int oneBarPatternIdx = 0;

    std::vector<Notes> chordProg;

    std::vector<std::vector<Notes>> chordProgs;

    Seq() {
        samplesPer16thNote = 5000;

        //                1           2           3           4
        oneBarPattern = { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 };
    }

    void setOneBarPattern(std::vector<int> _oneBarPattern) {
        oneBarPattern = _oneBarPattern;
    }

    void setChordProg(std::vector<Notes> _chordProg) {
        chordProg = _chordProg;
    }

    void setChordProgs(std::vector<std::vector<Notes>> _chordProgs) {
        chordProgs = _chordProgs;
    }

    bool trigger() {
        return (is16thNote() && oneBarPattern[_16ToM] == 1);
    }

    bool is16thNote() {
        return (sTo16 == 0);
    }

    // void run() override {
    //     sTo16Rollover = modInc(sTo16, samplesPer16thNote);
    //     if (sTo16Rollover) {
    //         _16ToMRollover = modInc(_16ToM, 16);
    //         if (_16ToMRollover) {
    //             measuresRollover = modInc(measures, chordProgs[chordProgCounter].size());
    //             if (measuresRollover) {
    //                 chordProgCounterRollover = modInc(chordProgCounter, chordProgs.size());
    //             }
    //         }
    //     }
    // }

    void run() override {
        sTo16Rollover = modInc(sTo16, samplesPer16thNote);
        if (sTo16Rollover) {
            _16ToMRollover = modInc(_16ToM, 16);
            if (_16ToMRollover) {
                measuresRollover = modInc(measures, chordProg.size());
            }
        }
    }
};

}
