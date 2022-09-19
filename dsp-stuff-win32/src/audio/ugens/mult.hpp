#pragma once

#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - in0 signal
// in[1]  - in1 signal
// out[0] - in0 * in1

class Mult : public BaseUgen {
public:
    Mult() {
        resizeIns(2);
        resizeOuts(1);
    }
    
    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; ++i) {
            writeOut(0, i, in[0][i] * in[1][i]);
        }
    }
};
