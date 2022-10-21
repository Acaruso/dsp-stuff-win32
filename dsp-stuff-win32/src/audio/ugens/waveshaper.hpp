#pragma once

#include "src/audio/ugens/base_ugen.hpp"

// in[0]  - in signal
// out[0] - out signal

class Waveshaper : public BaseUgen {
public:
    std::vector<float>* wavetable;
    int wtSize = 0;
    float inSig = 0.0f;
    float outSig = 0.0f;
    int idx = 0;
    float mult = 0.0f;

    Waveshaper(UgenCtx* _ugenCtx, std::vector<float>* _wavetable, float _mult = 1.0f) {
        typeStr = "Waveshaper";
        ugenCtx = _ugenCtx;
        wavetable = _wavetable;
        wtSize = wavetable->size();
        mult = _mult;
        numIns = 1;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            inSig = READ_IN(d, in0, i) * mult;

            // TODO: lerp this?
            idx = (int)(((inSig + 1) * 0.5) * wtSize);

            WRITE_OUT(d, out0, i, (*wavetable)[idx]);
        }
    }
};
