#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

struct ValueSeqCell {
    bool on = false;
    float value = 0.0f;
};

// out[n] - trigger value

class ValueSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned patternCounter = 0;
    unsigned n16len = 0;
    unsigned patternLen = 16;
    int numTracks = 0;
    bool on = false;

    std::vector<std::vector<ValueSeqCell>> patterns;

    ValueSeq(UgenCtx* _ugenCtx, unsigned _len16, int _numTracks) {
        typeStr = "ValueSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        numTracks = _numTracks;

        patterns.resize(numTracks, std::vector<ValueSeqCell>(16));

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

    void set(int patternIdx, int stepIdx, float value) {
        patterns[patternIdx][stepIdx] = { true, value };
    }

    void set(int patternIdx, int stepIdx) {
        patterns[patternIdx][stepIdx] = { true, 1.0f };
    }

    void unset(int patternIdx, int stepIdx) {
        patterns[patternIdx][stepIdx] = { false, 0.0f };
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
                            WRITE_OUT(
                                d, 
                                out[curOut], 
                                sampIdx, 
                                pattern[patternCounter].value
                            );
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
