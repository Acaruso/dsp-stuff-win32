#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"

// untested -- does this even work?

class Pass : public BaseUgen {
public:
    Pass(UgenCtx* _ugenCtx, int numInsOuts) {
        ugenCtx = _ugenCtx;
        numIns = numInsOuts;
        numOuts = numInsOuts;
        allocateBuffers("Pass");
    }
    
    AudioBuffer buffer = AudioBuffer(bufferSize, 0.0f);

    void run(unsigned sampleCounter) override {
        for (int inOutIdx = 0; inOutIdx < numIns; ++inOutIdx) {
            for (int sampleIdx = 0; sampleIdx < bufferSize; ++sampleIdx) {
                writeOut(
                    inOutIdx,
                    sampleIdx,
                    readIn(inOutIdx, sampleIdx)
                );
            }
        }
    }
};
