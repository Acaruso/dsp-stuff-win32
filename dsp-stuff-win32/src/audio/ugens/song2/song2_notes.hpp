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

    int base = 60;  // middle C
    // int base = 50;
    ScaleType scaleType = MAJOR;

    Freqs makeMajorChord(int root) {
        Freqs freqs;
        freqs.push(noteToFreq(base + root));
        freqs.push(noteToFreq(base + root + 4));
        freqs.push(noteToFreq(base + root + 7));
        freqs.push(noteToFreq(base + root - 24));
        return freqs;
    }

    Freqs makeMinorChord(int root) {
        Freqs freqs;
        freqs.push(noteToFreq(base + root));
        freqs.push(noteToFreq(base + root + 3));
        freqs.push(noteToFreq(base + root + 7));
        freqs.push(noteToFreq(base + root - 24));
        return freqs;
    }

    Freqs makeDimChord(int root) {
        Freqs freqs;
        freqs.push(noteToFreq(base + root));
        freqs.push(noteToFreq(base + root + 3));
        freqs.push(noteToFreq(base + root + 6));
        freqs.push(noteToFreq(base + root - 24));
        return freqs;
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

}
