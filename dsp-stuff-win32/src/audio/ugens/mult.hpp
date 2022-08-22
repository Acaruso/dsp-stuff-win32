#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Mult : public BaseUgen {
public:
    void get(double t) override {
        for (int i = 0; i < numOutputs; i++) {
            writeOutput(i, inputs[0]);
        }
    }
};
