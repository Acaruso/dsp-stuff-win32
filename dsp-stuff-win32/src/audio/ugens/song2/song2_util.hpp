#pragma once

#include <vector>

#include "src/audio/audio_util.hpp"

class Song2Util {
public:
    // 0 is root, 11 is root one octave up
    
    std::vector<int> notes = { 0, 3, 5, 8 };

    std::vector<int> noteBases = { 50, 53, 55, 60 };

    int noteBase = 50;

    int noteIdx = 0;
    int noteBaseIdx = 0;

    float getFreq() {
        // return noteToFreq(
        //     noteBase + notes[noteIdx]
        // );
        return noteToFreq(
            noteBases[noteBaseIdx] + notes[noteIdx]
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
