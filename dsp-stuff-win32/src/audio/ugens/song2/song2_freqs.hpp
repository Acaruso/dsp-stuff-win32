#pragma once

namespace Song2 {

const int FREQS_SIZE = 12;

struct Freqs {
    float f[FREQS_SIZE];
    int size = 0;

    Freqs() {}

    Freqs(float f1) {
        push(f1);
    }

    Freqs(float f1, float f2) {
        push(f1);
        push(f2);
    }

    Freqs(float f1, float f2, float f3) {
        push(f1);
        push(f2);
        push(f3);
    }

    Freqs(float f1, float f2, float f3, float f4) {
        push(f1);
        push(f2);
        push(f3);
        push(f4);
    }

    void push(float _f) {
        if (size < FREQS_SIZE) {
            f[size] = _f;
            ++size;
        }
    }
};

}
