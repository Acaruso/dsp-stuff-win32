#pragma once

#include "src/audio/ugens/base_ugen.hpp"

class Sink : public BaseUgen {
public:
    std::vector<double> buffer = std::vector<double>(bufferSize, 0.0);

    void run(unsigned sampleCounter) override {
        for (int i = 0; i < bufferSize; i++) {
            buffer[i] = in[0][i];
        }
    }
};
