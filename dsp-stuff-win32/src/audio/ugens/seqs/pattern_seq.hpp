#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

struct PatternSeqCell {
    bool on = false;
};

// out[n] - trigger

class PatternSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned patternCounter = 0;
    unsigned n16len = 0;
    unsigned patternLen = 16;
    int numTracks = 0;
    bool on = false;

    std::vector<std::vector<PatternSeqCell>> patterns;

    PatternSeq(UgenCtx* _ugenCtx, unsigned _len16, int _numTracks) {
        typeStr = "PatternSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        numTracks = _numTracks;

        patterns.resize(numTracks, std::vector<PatternSeqCell>(16));

        numIns = 0;
        numOuts = numTracks;
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
            for (int sampIdx = 0; sampIdx < bufferSize; ++sampIdx) {
                if (n16counter == 0) {
                    for (int curOut = 0; curOut < patterns.size(); ++curOut) {
                        auto& pattern = patterns[curOut];
                        if (pattern[patternCounter].on) {
                            WRITE_OUT(d, out[curOut], sampIdx, 1.0f);
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
