#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

class Sum : public BaseUgen {
public:
    Sum(UgenCtx* _ugenCtx, int _numIns) {
        ugenCtx = _ugenCtx;
        numIns = _numIns;
        numOuts = 1;
        allocateBuffers("Sum");
    }

    void zeroOut() {
        auto& d = ugenCtx->bufferAllocator.data;
        int out0 = out[0];
        fillVector(d, out0, bufferSize, 0.0f);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        int curIn = 0;
        int out0 = out[0];

        zeroOut();

        for (int inIdx = 0; inIdx < numIns; ++inIdx) {
            curIn = in[inIdx];
            for (int i = 0; i < bufferSize; i++) {
                d[out0 + i] += d[curIn + i];
            }
        }
    }
};
