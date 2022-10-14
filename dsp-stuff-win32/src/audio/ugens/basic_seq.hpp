#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - trigger

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

        fillBuffer(d, out0, bufferSize, 0.0f);

        for (int i = 0; i < bufferSize; ++i) {
            if ((sampleCounter + i) % 2000 == 0) {
            // if (sampleCounter % 40000 == 0) {
                WRITE_OUT(d, out0, 0, 1.0f);
            }
        }
    }
};
