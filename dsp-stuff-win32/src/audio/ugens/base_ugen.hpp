#pragma once

#include <algorithm>
#include <vector>

#include "src/audio/audio_constants.hpp"

class BaseUgen;

using Buffer = std::vector<double>;

class BaseUgen {
public:
    // TODO: how to determine this dynamically?
    int bufferSize = samplesPerSecond / 100;

    std::vector<Buffer> in;
    std::vector<std::vector<Buffer*>> out;

    inline void writeOut(int outIdx, int sampleIdx, double sample) {
        for (auto pBuffer : out[outIdx]) {
            (*pBuffer)[sampleIdx] += sample;
        }
    }

    void zeroIns() {
        for (auto& v : in) {
            std::fill(v.begin(), v.end(), 0.0);
        }
    }

    void resizeIns(int newSize) {
        in.resize(newSize, std::vector<double>(bufferSize, 0.0));
    }

    void resizeOuts(int newSize) {
        out.resize(newSize, std::vector<Buffer*>());
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
