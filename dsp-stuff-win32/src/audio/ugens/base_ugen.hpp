#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/shared/audio_buffer.hpp"

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

    // write directly to input buffer
    // typically, don't need to use this
    inline void writeIn(int inIdx, int sampleIdx, float sample) {
        int inOffset = in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] = sample;
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
