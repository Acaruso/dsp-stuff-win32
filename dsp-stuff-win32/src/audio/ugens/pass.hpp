#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"

class Pass : public BaseUgen {
public:
    Pass() {}

    Pass(int numIns, int numOuts) {
        resizeIns(numIns);
        resizeOuts(numOuts);
    }

    void run(unsigned sampleCounter) override {
        // for (int i = 0; i < out.size(); i++) {
        //     auto& pDestBuffers = out[i];

        //     for (AudioBuffer* pDestBuffer : pDestBuffers) {
        //         sumCopy(*pDestBuffer, outBuffers[i]);
        //     }
        // }
    }
};
