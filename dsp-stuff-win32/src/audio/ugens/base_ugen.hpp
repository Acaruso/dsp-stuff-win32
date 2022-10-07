#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"

#define READ_IN(data, offset, sampleIdx) data[offset + sampleIdx]

#define WRITE_OUT(data, offset, sampleIdx, sample) data[offset + sampleIdx] = sample

class BaseUgen {
public:
    int numIns = 0;
    int numOuts = 0;

    std::vector<unsigned> in;
    std::vector<unsigned> out;
    UgenCtx* ugenCtx = nullptr;

    virtual void allocateBuffers(std::string str="") {
        resizeIns(numIns, str);
        resizeOuts(numOuts, str);
    }

    void resizeIns(int newSize, std::string str) {
        for (int i = 0; i < newSize; i++) {
            unsigned newOffset = ugenCtx->bufferAllocator.allocate(str);
            in.push_back(newOffset);
        }
    }

    void resizeOuts(int newSize, std::string str) {
        out.resize(newSize, 0);
    }

    void addIn() {
        ++numIns;
        unsigned newOffset = ugenCtx->bufferAllocator.allocate();
        in.push_back(newOffset);
    }

    void zeroIns() {
        for (auto offset : in) {
            std::fill(
                ugenCtx->bufferAllocator.data.begin() + offset,
                ugenCtx->bufferAllocator.data.begin() + offset + bufferSize,
                0.0f
            );
        }
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
