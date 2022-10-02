#pragma once

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
        auto& data = ugenCtx->bufferAllocator.data;

        int outOffset = out[0];

        if (banging) {
            banging = false;
            data[outOffset] = 1.0f;
            std::fill(
                ugenCtx->bufferAllocator.data.begin() + outOffset + 1,
                ugenCtx->bufferAllocator.data.begin() + outOffset + bufferSize,
                0.0f
            );
        } else {
            std::fill(
                ugenCtx->bufferAllocator.data.begin() + outOffset,
                ugenCtx->bufferAllocator.data.begin() + outOffset + bufferSize,
                0.0f
            );
        }
    }
};
