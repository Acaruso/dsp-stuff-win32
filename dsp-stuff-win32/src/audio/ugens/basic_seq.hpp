#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - trigger

class BasicSeq : public BaseUgen {
public:
    int period = 0;
    unsigned counter = 0;
    bool on = false;

    BasicSeq(UgenCtx* _ugenCtx, int _period) {
        ugenCtx = _ugenCtx;
        period = _period;
        numIns = 0;
        numOuts = 1;
        allocateBuffers("BasicSeq");
    }

    void toggle() {
        if (on == false) {
            counter = 0;
            on = true;
        } else {
            on = false;
        }
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        fillBuffer(d, out0, bufferSize, 0.0f);

        if (on) {
            for (int i = 0; i < bufferSize; ++i) {
                if (counter >= period) {
                    WRITE_OUT(d, out0, i, 1.0f);
                    counter = 0;
                } else {
                    ++counter;
                }
            }
        }
    }
};
