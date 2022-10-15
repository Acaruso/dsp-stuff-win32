#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - trigger

#define SHOULD_TRIGGER(sampleCounter, i, period) (((sampleCounter) + (i)) % (period) == 0)

class BasicSeq : public BaseUgen {
public:
    int period = 0;

    BasicSeq(UgenCtx* _ugenCtx, int _period) {
        ugenCtx = _ugenCtx;
        period = _period;
        numIns = 0;
        numOuts = 1;
        allocateBuffers("BasicSeq");
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        fillBuffer(d, out0, bufferSize, 0.0f);

        for (int i = 0; i < bufferSize; ++i) {
            if (SHOULD_TRIGGER(sampleCounter, i, period)) {
                WRITE_OUT(d, out0, i, 1.0f);
            }
        }
    }
};
