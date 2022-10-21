#pragma once

#include <algorithm>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

class Split : public BaseUgen {
public:
    Split(UgenCtx* _ugenCtx, int _numOuts) {
        typeStr = "Split";
        ugenCtx = _ugenCtx;
        numIns = 1;
        numOuts = _numOuts;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        int in0 = in[0];
        int curOut = 0;

        for (int outIdx = 0; outIdx < numOuts; ++outIdx) {
            curOut = out[outIdx];
            copyBuffer(d, in0, bufferSize, curOut);
        }
    }
};
