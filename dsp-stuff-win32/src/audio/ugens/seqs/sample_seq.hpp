#pragma once

#include <functional>
#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

struct SampleSeqCell {
    bool valid = false;
    bool on = false;
    float value = 0.0f;
    unsigned sample = 0;
    std::function<void()> lambda = []() {};
};

struct SampleSeqTrack {
    static const int MAX_SIZE = 128;

    std::vector<SampleSeqCell> data = std::vector<SampleSeqCell>(
        MAX_SIZE,
        { false, false, 0.0f, 0, []() {}}
    );

    int size = 0;
    int curIdx = 0;

    SampleSeqCell& getCur() {
        return data[curIdx];
    }

    void incCur() {
        ++curIdx;
        if (curIdx == size) {
            curIdx = 0;
        }
    }

    void add(SampleSeqCell newCell) {
        if (size + 1 == MAX_SIZE) {
            // TODO: throw error
            return;
        }

        int insertIdx = 0;

        while (data[insertIdx].sample < newCell.sample) {
            ++insertIdx;
        }

        for (int i = size - 1; i >= insertIdx; --i) {
            data[i + 1] = data[i];
        }

        ++size;

        data[insertIdx] = newCell;
    }
};

// out[n] - trigger value

class SampleSeq : public BaseUgen {
public:
    unsigned counter = 0;
    unsigned len = 0;
    int numTracks = 0;
    bool on = false;

    std::vector<SampleSeqTrack> patterns;

    SampleSeq(UgenCtx* _ugenCtx, unsigned _len, int _numTracks) {
        typeStr = "SampleSeq";
        ugenCtx = _ugenCtx;
        len = _len;
        numTracks = _numTracks;

        patterns.resize(numTracks, SampleSeqTrack{});

        numIns = 0;
        numOuts = numTracks;
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

    void set(int patternIdx, SampleSeqCell newCell) {
        patterns[patternIdx].add(newCell);
    }

    // void unset(int patternIdx, int stepIdx) {
    //     patterns[patternIdx][stepIdx] = { false, 0.0f };
    
    // }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        for (int i = 0; i < patterns.size(); ++i) {
            fillBuffer(d, out[i], bufferSize, 0.0f);
        }

        if (on) {
            for (int sampIdx = 0; sampIdx < bufferSize; ++sampIdx) {
                for (int trackIdx = 0; trackIdx < patterns.size(); ++trackIdx) {
                    auto& track = patterns[trackIdx];
                    auto& cur = track.getCur();
                    if (cur.sample == sampIdx) {
                        // TODO: move lambda() before WRITE_OUT in LambdaSeq
                        cur.lambda();
                        
                        WRITE_OUT(
                            d, 
                            out[trackIdx], 
                            sampIdx, 
                            cur.value
                        );

                        track.incCur();
                    }
                }

                ++counter;
                if (counter >= len) {
                    counter = 0;
                }
            }
        }
    }
};
