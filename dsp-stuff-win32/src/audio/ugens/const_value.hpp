#pragma once

#include "src/audio/ugens/base_ugen.hpp"

// out[0] - signal

class ConstValue : public BaseUgen {
public:
    float value = 0.0f;

    ConstValue(UgenCtx* _ugenCtx, float _value) {
        ugenCtx = _ugenCtx;
        value = _value;
        resizeOuts(1);
    }

    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; ++i) {
            writeOut(0, i, value);
        }
    }
};
