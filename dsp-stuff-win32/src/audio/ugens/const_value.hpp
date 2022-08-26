#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class ConstValue : public BaseUgen {
public:
    double value = 0;

    ConstValue (double _value) {
        value = _value;
    }

    void run(double t) override {
        out[0] = value;
    }
};
