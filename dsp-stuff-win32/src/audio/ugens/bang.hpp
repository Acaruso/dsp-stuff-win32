#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

class Bang : public BaseUgen {
public:
    bool banging = false;

    Bang(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 1;
        numOuts = 1;
        allocateBuffers("Bang");
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
            fillVector(d, out0 + 1, bufferSize, 0.0f);
        } else {
            fillVector(d, out0, bufferSize, 0.0f);
        }
    }
};
