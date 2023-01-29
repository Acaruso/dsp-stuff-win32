#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Notes {
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

}
