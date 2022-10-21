#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// out[0] - signal

class ConstValue : public BaseUgen {
public:
    float value = 0.0f;

    ConstValue(UgenCtx* _ugenCtx, float _value) {
        typeStr = "ConstValue";
        ugenCtx = _ugenCtx;
        value = _value;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        fillBuffer(d, out[0], bufferSize, value);
    }
};
