#pragma once

namespace Song2 {

const int FREQS_SIZE = 12;

struct Freqs {
    float f[FREQS_SIZE];
    int size = 0;

    void push(float _f) {
        if (size < FREQS_SIZE) {
            f[size] = _f;
            ++size;
        }
    }
};

}
