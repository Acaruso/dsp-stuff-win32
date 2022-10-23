#pragma once

#include <iostream>

#include "src/audio/ugens/base_ugen.hpp"

class ConstMult : public BaseUgen {
public:
    ConstMult(UgenCtx* _ugenCtx, float _level) {
        typeStr = "ConstMult";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 1;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        unsigned in0 = in[0];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            WRITE_OUT(
                d,
                out0,
                i,
                READ_IN(d, in0, i) * level
            );
        }
    }
};
