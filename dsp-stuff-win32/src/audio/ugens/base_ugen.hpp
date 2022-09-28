#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/shared/audio_buffer.hpp"

struct UgenOut {
    std::vector<UgenOut*> children;
    std::vector<unsigned> bufferOffsets;
};

class BaseUgen {
public:
    int numIns = 0;
    int numOuts = 0;

    std::vector<unsigned> in;                    // offsets into ugenCtx buffer data
    std::vector<UgenOut> out;
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
        out.resize(newSize);
    }

    inline float readIn(int inIdx, int sampleIdx) {
        return bufRead(in[inIdx], sampleIdx);
    }

    inline void writeOut(int outIdx, int sampleIdx, float sample) {
        // for (auto pUgenOut : out[outIdx].children) {
        //     bufWrite(pUgenOut, sampleIdx, sample);
        // }

        bufWrite(&out[outIdx], sampleIdx, sample);
    }

    // write directly to input buffer
    // typically, don't need to use this
    inline void writeIn(int inIdx, int sampleIdx, float sample) {
        int inOffset = in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] += sample;
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

private:
    inline float bufRead(int offset, int i) {
        return ugenCtx->bufferAllocator.data[offset + i];
    }

    // inline void bufWrite(UgenOut* ugenOut, int i, float sample) {
    //     while (ugenOut->pNext != nullptr) {
    //         ugenOut = ugenOut->pNext;
    //     }

    //     ugenCtx->bufferAllocator.data[ugenOut->bufferOffset + i] += sample;
    // }

    inline void bufWrite(UgenOut* pUgenOut, int i, float sample) {
        for (auto pChild : pUgenOut->children) {
            bufWrite(pChild, i, sample);
        }

        for (auto bufferOffset : pUgenOut->bufferOffsets) {
            ugenCtx->bufferAllocator.data[bufferOffset + i] += sample;
        }
    }
};
