#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    int amount = 0;

    Mult(int _amount) {
        amount = _amount;
    }

    void run(double t) override {
        outSigs[0] = inSigs[0] * amount;
    }
};
