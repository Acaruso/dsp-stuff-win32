#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_freqs.hpp"
#include "src/main/util.hpp"

namespace Song2 {

enum ScaleType {
    MAJOR,
    MINOR
};

class Notes {
public:
    // minor scale: 0, 2, 3, 5, 7, 8, 10

    // int base = 60;  // middle C
    int base = 50;  // middle C
    ScaleType scaleType = MAJOR;

    Freqs makeMajorChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 4),
            noteToFreq(base + root + 7),
        };
    }

    Freqs makeMinorChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 3),
            noteToFreq(base + root + 7),
        };
    }

    Freqs makeDimChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 3),
            noteToFreq(base + root + 6),
        };
    }

    Freqs makeChord(int root) {
        if (root == 0) {
            // c maj
            return makeMajorChord(0);
        } else if (root == 1) {
            // d min
            return makeMinorChord(2);
        } else if (root == 2) {
            // e min
            return makeMinorChord(4);
        } else if (root == 3) {
            // f maj
            return makeMajorChord(5);
        } else if (root == 4) {
            // g maj
            return makeMajorChord(7);
        } else if (root == 5) {
            // a min
            return makeMinorChord(9);
        } else if (root == 6) {
            // b dim
            return makeDimChord(11);
        } else {
            // default
            return makeMajorChord(0);
        }
    }
};

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
