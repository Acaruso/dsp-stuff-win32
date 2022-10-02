#pragma once

#include "src/audio/ugens/base_ugen.hpp"


// TODO: test this

class Sum : public BaseUgen {
public:
    Sum(UgenCtx* _ugenCtx, int _numIns) {
        ugenCtx = _ugenCtx;
        numIns = _numIns;
        numOuts = 1;
        allocateBuffers("Sum");
    }

    void zeroOut() {
        auto& data = ugenCtx->bufferAllocator.data;
        int outOffset = out[0];
        std::fill(data.begin() + outOffset, data.begin() + outOffset + bufferSize, 0.0f);
    }

    void run(unsigned sampleCounter) override {
        zeroOut();

        auto& data = ugenCtx->bufferAllocator.data;

        int inOffset = 0;
        int outOffset = out[0];

        for (int inIdx = 0; inIdx < numIns; ++inIdx) {
            inOffset = in[inIdx];
            for (int i = 0; i < bufferSize; i++) {
                data[outOffset + i] += data[inOffset + i];
            }
        }
    }
};
