#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Seq {
public:
    int samplesPer16thNote;

    int sTo16;
    bool sTo16Rollover = false;

    int _16ToM;
    bool _16ToMRollover = false;

    int measures;
    int numMeasures;

    std::vector<int> oneBarPattern;
    int oneBarPatternIdx;

    Seq() {
        samplesPer16thNote = 5000;
        numMeasures = 8;

        //                1           2           3           4
        oneBarPattern = { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 };
    }

    void setOneBarPattern(std::vector<int> _oneBarPattern) {
        oneBarPattern = _oneBarPattern;
    }

    bool trigger() {
        return (is16thNote() && oneBarPattern[_16ToM] == 1);
    }

    bool is16thNote() {
        return (sTo16 == 0);
    }

    void run() {
        sTo16Rollover = modInc(sTo16, samplesPer16thNote);
        if (sTo16Rollover) {
            _16ToMRollover = modInc(_16ToM, 16);
            if (_16ToMRollover) {
                modInc(measures, numMeasures);
            }
        }
    }
};

}
