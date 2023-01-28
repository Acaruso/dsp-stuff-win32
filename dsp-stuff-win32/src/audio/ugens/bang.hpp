#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

/*
- purpose of Bang is to be triggered from the UI thread by sending AM_TRIG message to audio queue
- thus, Bang is triggered at `bufferSize` granularity
  - user clicks button -- Bang is triggered at beginning of next audio buffer
*/

class Bang : public BaseUgen {
public:
    bool banging = false;

    Bang(UgenCtx* _ugenCtx) {
        typeStr = "Bang";
        ugenCtx = _ugenCtx;
        numIns = 1;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void doBang() {
        banging = true;
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        int out0 = out[0];

        if (banging) {
            banging = false;
            d[out0] = 1.0f;
            fillBuffer(d, out0 + 1, bufferSize, 0.0f);
        } else {
            fillBuffer(d, out0, bufferSize, 0.0f);
        }
    }
};
