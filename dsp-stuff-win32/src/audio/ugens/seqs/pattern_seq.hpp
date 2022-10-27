#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

struct PatternSeqCell {
    bool on = false;
};

// out[0] - trigger

class PatternSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned patternCounter = 0;
    unsigned n16len = 0;
    unsigned patternLen = 0;
    bool on = false;

    std::vector<std::vector<PatternSeqCell>> patterns;

    PatternSeq(UgenCtx* _ugenCtx, unsigned _len16) {
        typeStr = "PatternSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        patternLen = 16;

        patterns.resize(2, std::vector<PatternSeqCell>(16));

        patterns[0][0] = PatternSeqCell{true};
        patterns[0][6] = PatternSeqCell{true};
        patterns[0][8] = PatternSeqCell{true};
        patterns[0][12] = PatternSeqCell{true};

        patterns[1][4] = PatternSeqCell{true};
        patterns[1][12] = PatternSeqCell{true};

        numIns = 0;
        numOuts = 4;
        allocateBuffers(typeStr);
    }

    void toggle() {
        if (on == false) {
            n16counter = 0;
            patternCounter = 0;
            on = true;
        } else {
            on = false;
        }
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        for (int i = 0; i < patterns.size(); ++i) {
            fillBuffer(d, out[i], bufferSize, 0.0f);
        }

        if (on) {
            for (int i = 0; i < bufferSize; ++i) {
                if (n16counter == 0) {
                    for (int i = 0; i < patterns.size(); ++i) {
                        auto& pattern = patterns[i];
                        if (pattern[patternCounter].on) {
                            WRITE_OUT(d, out[i], i, 1.0f);
                        }
                    }

                    ++patternCounter;
                    if (patternCounter >= patternLen) {
                        patternCounter = 0;
                    }
                }
                
                ++n16counter;

                if (n16counter >= n16len) {
                    n16counter = 0;
                }
            }
        }
    }
};
