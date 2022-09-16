#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class ConstValue : public BaseUgen {
public:
    double value = 0;

    ConstValue (double _value) {
        value = _value;
    }

    void run(unsigned sampleCounter) override {
        std::vector<double>* out0 = getOutPtr(&out[0]);

        for (int i = 0; i < bufferSize; ++i) {
            (*out0)[i] += value;
        }
    }
};
