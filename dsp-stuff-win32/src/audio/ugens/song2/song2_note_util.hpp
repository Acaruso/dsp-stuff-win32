#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/main/util.hpp"

namespace Song2 {

enum ScaleType {
    MAJOR,
    MINOR
};

class NoteUtil {
public:
    // minor scale: 0, 2, 3, 5, 7, 8, 10

    int base = 60;  // middle C
    // int base = 50;
    ScaleType scaleType = MAJOR;

    // Freqs makeMajorChord(int root) {
    //     return Freqs(
    //         noteToFreq(base + root),
    //         noteToFreq(base + root + 4),
    //         noteToFreq(base + root + 7),
    //         noteToFreq(base + root -24)
    //     );
    // }

    // Freqs makeMinorChord(int root) {
    //     return Freqs(
    //         noteToFreq(base + root),
    //         noteToFreq(base + root + 3),
    //         noteToFreq(base + root + 7),
    //         noteToFreq(base + root -24)
    //     );
    // }

    // Freqs makeDimChord(int root) {
    //     return Freqs(
    //         noteToFreq(base + root),
    //         noteToFreq(base + root + 3),
    //         noteToFreq(base + root + 6),
    //         noteToFreq(base + root -24)
    //     );
    // }

    Notes makeMajorChord(int root) {
        return Notes(
            base + root,
            base + root + 4,
            base + root + 7,
            base + root - 24
        );
    }

    Notes makeMinorChord(int root) {
        return Notes(
            base + root,
            base + root + 3,
            base + root + 7,
            base + root - 24
        );
    }

    Notes makeDimChord(int root) {
        return Notes(
            base + root,
            base + root + 3,
            base + root + 6,
            base + root - 24
        );
    }

    Notes addOctaves(Notes notes) {
        Notes newNotes = notes;

        for (int i = 0; i < notes.size; i++) {
            newNotes.push(
                notes.get(i) + 12
            );
        }

        return newNotes;
    }
};

}
