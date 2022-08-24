#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    void run(double t) override {
        outSigs[0] = inSigs[0] * inSigs[1];
    }
};
