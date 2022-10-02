#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Split : public BaseUgen {
public:
    Split(UgenCtx* _ugenCtx, int _numOuts) {
        ugenCtx = _ugenCtx;
        numIns = 1;
        numOuts = _numOuts;
        allocateBuffers("Split");
    }

    void run(unsigned sampleCounter) override {
        auto& data = ugenCtx->bufferAllocator.data;

        int inOffset = in[0];
        int outOffset = 0;

        for (int outIdx = 0; outIdx < numOuts; ++outIdx) {
            outOffset = out[outIdx];
            for (int i = 0; i < bufferSize; i++) {
                data[outOffset + i] = data[inOffset + i];
            }
        }
    }
};
