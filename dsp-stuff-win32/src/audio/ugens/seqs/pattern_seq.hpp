#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - trigger

class PatternSeq : public BaseUgen {
public:
    unsigned counter = 0;
    unsigned len16 = 0;
    unsigned idx = 0;
    bool on = false;

    // std::vector<std::string> pattern;
    std::vector<std::vector<std::string>> patterns;

    PatternSeq(UgenCtx* _ugenCtx, unsigned _len16) {
        typeStr = "PatternSeq";
        ugenCtx = _ugenCtx;
        len16 = _len16;

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
            counter = 0;
            on = true;
        } else {
            on = false;
        }
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        // unsigned out0 = out[0];
        // unsigned out1 = out[1];
        // unsigned out2 = out[2];
        // unsigned out3 = out[3];

        // fillBuffer(d, out0, bufferSize, 0.0f);

        for (int i = 0; i < patterns.size(); ++i) {
            fillBuffer(d, out[i], bufferSize, 0.0f);
        }

        if (on) {
            for (int i = 0; i < bufferSize; ++i) {
                if (counter % len16 == 0) {
                    idx = (counter / len16) % 16;

                    for (int i = 0; i < patterns.size(); ++i) {
                        auto& pattern = patterns[i];
                        if (pattern[idx] == ".") {
                            WRITE_OUT(d, out[i], i, 1.0f);
                        }
                    }

                    // for (auto& pattern : patterns) {
                    //     if (pattern[idx] == ".") {
                    //         WRITE_OUT(d, out0, i, 1.0f);
                    //     }
                    // }

                    // if (patterns[0][idx] == ".") {
                    //     WRITE_OUT(d, out0, i, 1.0f);
                    // }
                }
                ++counter;
            }
        }
    }
};
