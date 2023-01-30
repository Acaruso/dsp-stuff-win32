#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class Notes {
public:
    // minor scale
    // note 0 is root note, note 12 is root note one octave up
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
    int noteBaseIdx = 1;

    float getFreq() {
        return noteToFreq(
            (noteBases[noteBaseIdx] + notes[noteIdx])
        );
    }

    float getFreq(int i) {
        return noteToFreq(
            (noteBases[noteBaseIdx] + notes[i % notes.size()])
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

class Seq {
public:
    unsigned counter16thNote;
    unsigned samplesPer16thNote;
    std::vector<unsigned> trigCounters;

    Seq() {
        samplesPer16thNote = 16000;
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
