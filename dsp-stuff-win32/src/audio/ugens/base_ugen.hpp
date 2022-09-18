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

    std::vector<std::vector<double>> in = {
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0)
    };

    std::vector<std::vector<Buffer*>> out = {
        std::vector<Buffer*>(0),
        std::vector<Buffer*>(0),
        std::vector<Buffer*>(0),
        std::vector<Buffer*>(0)
    };

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

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
