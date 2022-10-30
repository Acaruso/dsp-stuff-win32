#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - trig value
// out[0] - const signal

class TrigToConstValue : public BaseUgen {
public:
    float inValue = 0.0f;
    float value = 0.0f;

    TrigToConstValue(UgenCtx* _ugenCtx, float _value) {
        typeStr = "TrigToConstValue";
        ugenCtx = _ugenCtx;
        value = _value;
        numIns = 1;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            inValue = READ_IN(d, in0, i);
            
            if (inValue != 0.0f) {
                value = inValue;
            }

            WRITE_OUT(d, out0, i, value);
        }
    }
};
