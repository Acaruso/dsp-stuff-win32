#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - trigger

class PatternSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned patternCounter = 0;
    unsigned n16len = 0;
    unsigned patternLen = 0;
    bool on = false;

    // std::vector<std::string> pattern;
    std::vector<std::vector<std::string>> patterns;

    PatternSeq(UgenCtx* _ugenCtx, unsigned _len16) {
        typeStr = "PatternSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        patternLen = 16;

        patterns.resize(4, std::vector<std::string>(16, "-"));

        //              *    -    -    -    *    -    -    -    *    -    -    -    *    -    -    -
        patterns[0] = {".", "-", "-", "-", "-", "-", ".", "-", ".", "-", "-", "-", ".", "-", "-", "-" };

        //              *    -    -    -    *    -    -    -    *    -    -    -    *    -    -    -
        patterns[1] = {"-", "-", "-", "-", ".", "-", "-", "-", "-", "-", "-", "-", ".", "-", "-", "-" };

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

    // void run(unsigned sampleCounter) override {
    //     auto& d = ugenCtx->bufferAllocator.data;

    //     for (int i = 0; i < patterns.size(); ++i) {
    //         fillBuffer(d, out[i], bufferSize, 0.0f);
    //     }

    //     if (on) {
    //         for (int i = 0; i < bufferSize; ++i) {
    //             if (counter % len16 == 0) {
    //                 idx = (counter / len16) % 16;

    //                 for (int i = 0; i < patterns.size(); ++i) {
    //                     auto& pattern = patterns[i];
    //                     if (pattern[idx] == ".") {
    //                         WRITE_OUT(d, out[i], i, 1.0f);
    //                     }
    //                 }
    //             }
    //             ++counter;
    //         }
    //     }
    // }

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
                        if (pattern[patternCounter] == ".") {
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
