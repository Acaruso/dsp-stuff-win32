#pragma once

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
    
    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; ++i) {
            writeOut(0, i, readIn(0, i) * readIn(1, i));
        }
    }
};
