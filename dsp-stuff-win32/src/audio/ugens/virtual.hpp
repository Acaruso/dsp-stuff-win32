#pragma once

#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/pass.hpp"

class Virtual : public BaseUgen {
public:
    int vIn = 0;
    int vOut = 0;
    BaseUgen* pVIn = nullptr;
    BaseUgen* pVOut = nullptr;

    Virtual(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 0;
        numOuts = 0;
        isVirtual = true;
        allocateBuffers("Virtual");
    }

    // void connectIn(int inBufferOffset, int inPort) {
    //     in[inPort] = inBufferOffset;
    // }

    // void connectOut(int outBufferOffset, int outPort) {
    //     out[outPort].push_back(outBufferOffset);
    // }

    void writeIn(int inIdx, int sampleIdx, float sample) override {
        int inOffset = pVIn->in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] += sample;
    }

    void run(unsigned sampleCounter) override {
        // do nothing
    }
};
