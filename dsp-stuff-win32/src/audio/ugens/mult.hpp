#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; ++i) {
            writeOut(0, i, in[0][i] * in[1][i]);
        }
    }
};
