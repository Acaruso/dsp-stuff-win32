#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/shared/audio_buffer.hpp"

class BaseUgen {
public:
    std::vector<unsigned> in;                  // offsets into ugenCtx buffer data
    std::vector<std::vector<unsigned>> out;
    UgenCtx* ugenCtx = nullptr;

    inline float readIn(int inIdx, int sampleIdx) {
        return bufRead(in[inIdx], sampleIdx);
    }

    inline void writeOut(int outIdx, int sampleIdx, float sample) {
        for (auto offset : out[outIdx]) {
            bufWrite(offset, sampleIdx, sample);

            // (*pBuffer)[sampleIdx] += sample;
        }
    }

    // write directly to input buffer
    // typically, don't need to use this
    inline void writeIn(int inIdx, int sampleIdx, float sample) {
        int inOffset = in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] += sample;
    }

    void zeroIns() {
        // for (auto& v : in) {
        //     std::fill(v.begin(), v.end(), 0.0f);
        // }
    }

    void resizeIns(int newSize) {
        for (int i = 0; i < newSize; i++) {
            unsigned newOffset = ugenCtx->bufferAllocator.allocate();
            in.push_back(newOffset);
        }
    }

    void resizeOuts(int newSize) {
        out.resize(newSize, std::vector<unsigned>());
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;

private:
    inline void bufWrite(int offset, int i, float sample) {
        ugenCtx->bufferAllocator.data[offset + i] += sample;
    }

    inline float bufRead(int offset, int i) {
        return ugenCtx->bufferAllocator.data[offset + i];
    }
};
