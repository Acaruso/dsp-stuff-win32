#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Splitter : public BaseUgen {
public:
    int numOutputs = 0;

    Splitter(int _numOutputs) {
        numOutputs = _numOutputs;
    }

    void _run(double t) override {
        for (int i = 0; i < numOutputs; i++) {
            outSigs[i] = inSigs[0];
        }
    }
};
