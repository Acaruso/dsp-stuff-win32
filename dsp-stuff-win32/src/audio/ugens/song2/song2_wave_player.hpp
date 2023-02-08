#pragma once

#include <iostream>
#include <vector>

#include "src/audio/ugens/song2/song2_base_gen.hpp"

namespace Song2 {

class WavePlayer : public BaseGen {
public:
    std::vector<float>* wave;
    int size = 0;
    int i = 0;

    WavePlayer(std::vector<float>* _wave) {
        wave = _wave;
        size = wave->size();
    }

    void trigger() {
        i = 0;
    }

    float get() {
        if (i < size) {
            return (*wave)[i];
        } else {
            return 0.0f;
        }
    }

    void run() override {
        ++i;
    }
};

}
