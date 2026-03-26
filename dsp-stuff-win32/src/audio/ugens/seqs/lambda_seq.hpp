#pragma once

#include <functional>
#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// TODO: test all of this

struct LambdaSeqCell {
    bool on = false;
    float value = 0.0f;
    std::function<void(int trackIdx, int stepIdx, LambdaSeqCell& cell)> lambda = [](
        int trackIdx,
        int stepIdx,
        LambdaSeqCell& cell
    ) {};
};

// out[n] - trigger value

class LambdaSeq : public BaseUgen {
public:
    unsigned n16counter = 0;
    unsigned stepIdx = 0;
    unsigned n16len = 0;
    unsigned patternLen = 16;
    int numTracks = 0;
    bool on = false;

    std::vector<std::vector<LambdaSeqCell>> patterns;

    LambdaSeq(UgenCtx* _ugenCtx, unsigned _len16, int _numTracks) {
        typeStr = "LambdaSeq";
        ugenCtx = _ugenCtx;
        n16len = _len16;
        numTracks = _numTracks;

        patterns.resize(numTracks, std::vector<LambdaSeqCell>(16));

        numIns = 0;
        numOuts = numTracks;
        allocateBuffers(typeStr);
    }

    std::vector<LambdaSeqCell>& getTrack(int trackIdx) {
        return patterns[trackIdx];
    }

    void toggle() {
        if (on == false) {
            n16counter = 0;
            stepIdx = 0;
            on = true;
        } else {
            on = false;
        }
    }

    void set(int patternIdx, int stepIdx) {
        patterns[patternIdx][stepIdx] = { true, 1.0f };
    }

    void set(int patternIdx, int stepIdx, std::function<void(int trackIdx, int stepIdx, LambdaSeqCell& cell)> lambda) {
        patterns[patternIdx][stepIdx] = { true, 1.0f, lambda };
    }

    void set(int patternIdx, int stepIdx, float value) {
        patterns[patternIdx][stepIdx] = { true, value };
    }

    void set(int patternIdx, int stepIdx, float value, std::function<void(int trackIdx, int stepIdx, LambdaSeqCell& cell)> lambda) {
        patterns[patternIdx][stepIdx] = { true, value, lambda };
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
                    for (int trackIdx = 0; trackIdx < patterns.size(); ++trackIdx) {
                        auto& track = patterns[trackIdx];
                        if (track[stepIdx].on) {
                            track[stepIdx].lambda(trackIdx, stepIdx, track[stepIdx]);
                            WRITE_OUT(
                                d,
                                out[trackIdx],
                                sampIdx,
                                track[stepIdx].value
                            );
                        }
                    }

                    ++stepIdx;
                    if (stepIdx >= patternLen) {
                        stepIdx = 0;
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
