#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"

// in[1] - input signal

class Sink : public BaseUgen {
public:
    Sink(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 1;
        allocateBuffers("Sink");
    }
    
    AudioBuffer buffer = AudioBuffer(bufferSize, 0.0f);

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];

        for (int i = 0; i < bufferSize; i++) {
            buffer[i] = READ_IN(d, in0, i);
        }
    }
};
