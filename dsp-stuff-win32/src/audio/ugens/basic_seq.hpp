#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"

class BasicSeq : public BaseUgen {
public:
    BasicSeq(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 0;
        numOuts = 1;
        allocateBuffers("BasicSeq");
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (sampleCounter % 4000 == 0) {
                WRITE_OUT(d, out0, 0, 1);
            }
        }
    }
};
