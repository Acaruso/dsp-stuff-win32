#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - in0 signal
// in[1]  - in1 signal
// out[0] - in0 * in1

class Mult : public BaseUgen {
public:
    Mult(UgenCtx* _ugenCtx) {
        typeStr = "Mult";
        ugenCtx = _ugenCtx;
        numIns = 2;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        unsigned in0 = in[0];
        unsigned in1 = in[1];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            WRITE_OUT(
                d, 
                out0, 
                i, 
                READ_IN(d, in0, i) * READ_IN(d, in1, i)
            );
        }
    }
};
