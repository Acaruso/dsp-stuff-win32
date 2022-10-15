#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - signal
// out[0] - scaled signal

class Scale : public BaseUgen {
public:
    float inLow;
    float inHigh;
    float outLow;
    float outHigh;

    float ratio;
    float offset;

    Scale(UgenCtx* _ugenCtx, float _inLow, float _inHigh, float _outLow, float _outHigh) {
        ugenCtx = _ugenCtx;
        inLow = _inLow;
        inHigh = _inHigh;
        outLow = _outLow;
        outHigh = _outHigh;

        ratio = (outHigh - outLow) / (inHigh - inLow);
        offset = outLow - (inLow * ratio);

        numIns = 1;
        numOuts = 1;
        allocateBuffers("Scale");
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            WRITE_OUT(
                d,
                out0, 
                i, 
                (READ_IN(d, in0, i) * ratio) + offset
            );
        }
    }
};
