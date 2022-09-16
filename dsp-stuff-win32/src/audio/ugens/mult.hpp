#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    void run(unsigned sampleCounter) override {
        std::vector<double>* out0 = getOutPtr(&out[0]);

        for (int i = 0; i < bufferSize; ++i) {
            (*out0)[i] += in[0][i] * in[1][i];
        }
    }
};
