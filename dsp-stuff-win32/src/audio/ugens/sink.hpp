#pragma once

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"

// in[1] - input signal

class Sink : public BaseUgen {
public:
    Sink() {
        resizeIns(1);
    }
    
    AudioBuffer buffer = AudioBuffer(bufferSize, 0.0);

    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; i++) {
            buffer[i] = in[0][i];
        }
    }
};
