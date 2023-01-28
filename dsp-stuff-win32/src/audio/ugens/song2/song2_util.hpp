#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

class Song2Util {
public:
    // 0 is root, 12 is root one octave up

    // minor scale
    std::vector<int> notes = { 
        0,
        2,
        3,
        5,
        7,
        8,
        10
    };

    std::vector<int> noteBases = { 50, 74 };

    int noteBase = 50;

    int noteIdx = 0;
    int noteBaseIdx = 0;

    float getFreq() {
        // return noteToFreq(
        //     noteBase + notes[noteIdx]
        // );
        return noteToFreq(
            (noteBases[noteBaseIdx] + notes[noteIdx])
        );
    }

    void incNote() {
        noteIdx++;
        if (noteIdx >= notes.size()) {
            incNoteBase();
            noteIdx = 0;
        }
    }

    void incNoteBase() {
        noteBaseIdx++;
        if (noteBaseIdx >= noteBases.size()) {
            noteBaseIdx = 0;
        }
    }
};

class Song2Saw {
public:
    float freq;
    float samp;
    float inc;
    unsigned counter;

    Song2Saw() {}

    Song2Saw(float _freq) {
        freq = _freq;
    }

    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void run() {
        ++counter;
        samp += inc;
        if (samp > 1.0f) {
            samp = 0.0f;
        }
    }

    float get() {
        if (counter % 8 == 0) {
            return 0.0f;
        }
        // if (counter % 4 == 0) {
        //     return 0.5f;
        // }
        return _round(
            (samp * 2.0f) - 1.0f,
            1
        );
    }
};
