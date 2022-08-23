#pragma once

#include <iostream>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"

class Recorder : public BaseUgen {
public:
    std::vector<double>* buffer = nullptr;
    int idx = 0;

    Recorder(std::vector<double>* _buffer) {
        buffer = _buffer;
    }

    void run(double t) {
        double sig = inSigs[0];
        double on = inSigs[1];

        if (on == 1.0) {
            if (idx < buffer->size()) {
                (*buffer)[idx] = sig;
                idx++;
            }
        } else {
            idx = 0;
        }
    }
};
