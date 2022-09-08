#pragma once

#include <iostream>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"

class Recorder : public BaseUgen {
public:
    std::vector<double>* buffer = nullptr;
    int idx = 0;
    double on = 0.0;

    Recorder(std::vector<double>* _buffer) {
        buffer = _buffer;
    }

    // in[0] - input
    // in[1] - on/off

    void run(unsigned sampleCounter) {
        on = in[1][0];

        if (on == 1.0) {
            for (int i = 0; i < bufferSize; ++i) {
                if (idx < buffer->size()) {
                    (*buffer)[idx] = in[0][i];
                    idx++;
                }
            }
        } else {
            idx = 0;
        }
    }
};
