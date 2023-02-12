#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class SimpleSeq : public BaseGen {
public:
    // sample counter
    int sTo16 = 0;
    int samplesPer16thNote = 5000;
    bool sTo16Rollover = false;

    // 16th note counter
    int _16ToM = 0;
    bool _16ToMRollover = false;

    std::vector<int> oneBarPattern{16, 0};
    int oneBarPatternIdx = 0;

    SimpleSeq() {}

    SimpleSeq(int _samplesPer16thNote) {
        samplesPer16thNote = _samplesPer16thNote;
    }

    SimpleSeq(std::vector<int> _oneBarPattern) {
        oneBarPattern = _oneBarPattern;
    }

    SimpleSeq(int _samplesPer16thNote, std::vector<int> _oneBarPattern) {
        samplesPer16thNote = _samplesPer16thNote;
        oneBarPattern = _oneBarPattern;
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

    void run() override {
        sTo16Rollover = modInc(sTo16, samplesPer16thNote);
        if (sTo16Rollover) {
            _16ToMRollover = modInc(_16ToM, oneBarPattern.size());
        }
    }
};

class Seq : public BaseGen {
public:
    int samplesPer16thNote = 5000;

    // sample counter
    int sTo16 = 0;
    bool sTo16Rollover = false;

    // 16th note counter
    int _16ToM = 0;
    bool _16ToMRollover = false;

    // measure counter
    int measures = 0;
    bool measuresRollover = false;

    int chordProgCounter = 0;
    bool chordProgCounterRollover = false;

    std::vector<int> oneBarPattern{16, 0};
    int oneBarPatternIdx = 0;

    std::vector<std::vector<Notes>> chordProgs;

    Seq() {}

    Seq(int _samplesPer16thNote) {
        samplesPer16thNote = _samplesPer16thNote;
    }

    Seq(std::vector<int> _oneBarPattern) {
        oneBarPattern = _oneBarPattern;
    }

    Seq(int _samplesPer16thNote, std::vector<int> _oneBarPattern) {
        samplesPer16thNote = _samplesPer16thNote;
        oneBarPattern = _oneBarPattern;
    }

    Seq(
        int _samplesPer16thNote, 
        std::vector<int> _oneBarPattern,
        std::vector<std::vector<Notes>> _chordProgs
    ) {
        samplesPer16thNote = _samplesPer16thNote;
        oneBarPattern = _oneBarPattern;
        chordProgs = _chordProgs;
    }

    void setOneBarPattern(std::vector<int>& _oneBarPattern) {
        oneBarPattern = _oneBarPattern;
    }

    void setChordProgs(std::vector<std::vector<Notes>>& _chordProgs) {
        chordProgs = _chordProgs;
    }

    Notes getCurChord() {
        return getCurChordProg()[measures];
    }

    std::vector<Notes>& getCurChordProg() {
        return chordProgs[chordProgCounter];
    }

    bool trigger() {
        return (is16thNote() && oneBarPattern[_16ToM] == 1);
    }

    bool is16thNote() {
        return (sTo16 == 0);
    }

    void run() override {
        sTo16Rollover = modInc(sTo16, samplesPer16thNote);
        if (sTo16Rollover) {
            _16ToMRollover = modInc(_16ToM, oneBarPattern.size());
            if (_16ToMRollover) {
                measuresRollover = modInc(measures, getCurChordProg().size());
                if (measuresRollover) {
                    chordProgCounterRollover = modInc(chordProgCounter, chordProgs.size());
                }
            }
        }
    }
};

// 384th notes    to        x
// 384            to        1 measure
// 192            to        1/2   note
// 96             to        1/4   note
// 48             to        1/8   note
// 32             to        1/8   note triplets (quarter note / 3)
// 24             to        1/16  note
// 16             to        1/16  note triplets (8th note / 3)
// 12             to        1/32  note
// 6              to        1/64  note
// 3              to        1/128 note
class AdvancedSeq : public BaseGen {
public:
    // 96 PPQ == 96 pulses per quarter note
    // each pulse is a 384th note
    // 96 * 4 = 384
    int samplesPer384thNote = 10;

    int sTo384 = 0;
    bool sTo384Rollover = false;

    int _384ToM = 0;
    bool _384ToMRollover = false;

    std::vector<int> events = std::vector<int>(128, 0);
    int eventsSize = 0;

    AdvancedSeq(int _samplesPer384thNote) {
        samplesPer384thNote = _samplesPer384thNote;
    }

    AdvancedSeq(int _samplesPer384thNote, std::vector<int> pattern) {
        samplesPer384thNote = _samplesPer384thNote;
        add16thNotePattern(pattern);
    }

    void add16thNotePattern(std::vector<int>& pattern) {
        for (int i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == 1) {
                add16thNote(i);
            } else if (pattern[i] == 2) {
                std::cout << "32 notes" << std::endl;
                add32ndNote(i * 2);
                add32ndNote((i * 2) + 1);
                add32ndNote((i * 2) + 2);
                add32ndNote((i * 2) + 3);
            }
        }

        printEvents();
    }

    void add16thNote(int pos) {
        addEvent(pos * 24);
    }

    void add32ndNote(int pos) {
        addEvent(pos * 12);
    }

    void add64thNote(int pos) {
        addEvent(pos * 6);
    }

    void addEvent(int pos) {
        events[eventsSize] = pos;
        ++eventsSize;
        sort(events.begin(), events.begin() + eventsSize);
    }

    bool trigger() {
        if (is384thNote()) {
            for (int i = 0; i < eventsSize; i++) {
                if (events[i] == _384ToM) {
                    return true;
                }
            }
        }
        return false;
    }

    bool is384thNote() {
        return (sTo384 == 0);
    }

    void printEvents() {
        for (int i = 0; i < eventsSize; ++i) {
            std::cout << events[i] << " ";
        }
        std::cout << std::endl;
    }

    void run() override {
        sTo384Rollover = modInc(sTo384, samplesPer384thNote);
        if (sTo384Rollover) {
            modInc(_384ToM, 384);
        }
    }
};

}
