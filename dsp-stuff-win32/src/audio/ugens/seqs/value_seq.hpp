#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

struct ValueSeqCell {
    bool on = false;
    float value = 0.0f;
};

enum ValueSeqPatternType {
    VS_TRIG,
    VS_CONST
};

struct ValueSeqPattern {
    ValueSeqPattern() {}

    ValueSeqPattern(int size) {
        data.resize(size);
    }

    ValueSeqPatternType type = VS_TRIG;
    std::vector<ValueSeqCell> data;
};

// out[n] - values

class ValueSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned patternCounter = 0;
    unsigned n16len = 0;
    unsigned patternLen = 16;
    int numTracks = 0;
    bool on = false;

    std::vector<ValueSeqPattern> patterns;

    ValueSeq(UgenCtx* _ugenCtx, unsigned _len16, int _numTracks) {
        typeStr = "ValueSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        numTracks = _numTracks;

        patterns.resize(numTracks, ValueSeqPattern(16));

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
            for (int i = 0; i < bufferSize; ++i) {
                if (n16counter == 0) {
                    for (int i = 0; i < patterns.size(); ++i) {
                        auto& pattern = patterns[i];
                        if (pattern.data[patternCounter].on) {
                            WRITE_OUT(d, out[i], i, pattern.data[patternCounter].value);
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
