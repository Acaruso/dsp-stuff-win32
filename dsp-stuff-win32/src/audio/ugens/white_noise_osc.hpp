#pragma once

#include "src/audio/ugens/base_ugen.hpp"

// out[0] - signal

class WhiteNoiseOsc : public BaseUgen {
public:
    float scale = 2.0f / (float)0xffffffff;
    int x1 = 0x67452301;
    int x2 = 0xefcdab89;

    WhiteNoiseOsc(UgenCtx* _ugenCtx) {
        typeStr = "WhiteNoiseOsc";
        ugenCtx = _ugenCtx;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            x1 ^= x2;
            WRITE_OUT(d, out0, i, x2 * scale * level);
            x2 += x1;
        }
    }
};
