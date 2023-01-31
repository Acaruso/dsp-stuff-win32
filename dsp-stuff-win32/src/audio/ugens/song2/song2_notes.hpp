#pragma once

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
    
    int base = 60;  // middle C
    ScaleType scaleType = MAJOR;

    Freqs makeMajorChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 4),
            noteToFreq(base + root + 7)
        };
    }

    Freqs makeMinorChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 3),
            noteToFreq(base + root + 7)
        };
    }

    Freqs makeDimChord(int root) {
        return Freqs {
            noteToFreq(base + root),
            noteToFreq(base + root + 3),
            noteToFreq(base + root + 6)
        };
    }

    Freqs makeChord(int root) {
        if (root == 0) {
            // c maj
            return makeMajorChord(root);
        } else if (root == 1) {
            // d min
            return makeMinorChord(root);
        } else if (root == 2) {
            // e min
            return makeMinorChord(root);
        } else if (root == 3) {
            // f maj
            return makeMajorChord(root);
        } else if (root == 4) {
            // g maj
            return makeMajorChord(root);
        } else if (root == 5) {
            // a min
            return makeMinorChord(root);
        } else if (root == 6) {
            // b dim
            return makeDimChord(root);
        } else {
            // default
            return makeMajorChord(root);
        }
    }
};

class Seq {
public:
    unsigned counter16thNote;
    unsigned samplesPer16thNote;
    std::vector<unsigned> trigCounters;

    Seq() {
        samplesPer16thNote = 100000;
        trigCounters = { 4 };
    }

    bool get() {
        return (counter16thNote == 0);
    }

    void run() {
        ++counter16thNote;
        if (counter16thNote == samplesPer16thNote) {
            counter16thNote = 0;
        }
    }
};

}
