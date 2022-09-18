#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class ConstValue : public BaseUgen {
public:
    double value = 0;

    ConstValue (double _value) {
        value = _value;
    }

    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; ++i) {
            writeOut(0, i, value);
        }
    }
};
