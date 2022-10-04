#pragma once

#include <algorithm>

#include "src/audio/ugens/base_ugen.hpp"

// out[0] - signal

class ConstValue : public BaseUgen {
public:
    float value = 0.0f;

    ConstValue(UgenCtx* _ugenCtx, float _value) {
        ugenCtx = _ugenCtx;
        value = _value;
        numOuts = 1;
        allocateBuffers("ConstValue");
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        std::fill(
            d.begin() + out0,
            d.begin() + out0 + bufferSize,
            value
        );
    }
};
