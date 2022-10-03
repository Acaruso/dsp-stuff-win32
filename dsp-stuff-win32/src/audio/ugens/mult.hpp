#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - in0 signal
// in[1]  - in1 signal
// out[0] - in0 * in1

class Mult : public BaseUgen {
public:
    Mult(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 2;
        numOuts = 1;
        allocateBuffers("Mult");
    }

    // TODO: do more optomizations like this
    //       ie, instead of calling writeOut which looks up in[0] and out[0] for every sample,
    //       look them up once per block and cache them, like we're doing below

    void run(unsigned sampleCounter) override {
        auto& data = ugenCtx->bufferAllocator.data;

        unsigned inOffset0 = in[0];
        unsigned inOffset1 = in[1];
        unsigned outOffset = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            data[outOffset + i] = data[inOffset0 + i] * data[inOffset1 + i];
        }
    }
};
