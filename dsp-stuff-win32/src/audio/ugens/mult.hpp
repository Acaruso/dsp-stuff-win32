#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    void run(double t) override {
        out[0] = in[0] * in[1];
    }
};
