#pragma once

#include <algorithm>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/shared/audio_buffer.hpp"

class BaseUgen {
public:
    // TODO: how to determine this dynamically?
    int bufferSize = samplesPerSecond / 100;

    std::vector<AudioBuffer> in;
    std::vector<std::vector<AudioBuffer*>> out;

    inline void writeOut(int outIdx, int sampleIdx, float sample) {
        for (auto pBuffer : out[outIdx]) {
            (*pBuffer)[sampleIdx] += sample;
        }
    }

    void zeroIns() {
        for (auto& v : in) {
            std::fill(v.begin(), v.end(), 0.0f);
        }
    }

    void resizeIns(int newSize) {
        in.resize(newSize, AudioBuffer(bufferSize, 0.0f));
    }

    void resizeOuts(int newSize) {
        out.resize(newSize, std::vector<AudioBuffer*>());
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
