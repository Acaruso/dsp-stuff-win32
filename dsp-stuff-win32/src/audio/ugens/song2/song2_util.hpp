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
    int noteIdx = 0;

    std::vector<int> noteBases = { 50, 74 };
    int noteBaseIdx = 0;

    float getFreq() {
        return noteToFreq(
            (noteBases[noteBaseIdx] + notes[noteIdx])
        );
    }

    void incNote() {
        noteIdx += 3;
        if (noteIdx >= notes.size()) {
            incNoteBase();
            noteIdx = noteIdx % notes.size();
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

    Song2Saw() {}

    Song2Saw(float _freq) {
        freq = _freq;
    }

    // if inc == secondsPerSample, we will oscillate one time per second
    // if inc == (secondsPerSampe * freq), we will oscillate `freq` times per second
    void setFreq(float _freq) {
        freq = _freq;
        inc = secondsPerSample * freq;
    }

    void run() {
        samp += inc;
        if (samp > 1.0f) {
            samp = samp - 1.0f;
        }
    }

    float get() {
        return (samp * 2.0f) - 1.0f;
    }
};
