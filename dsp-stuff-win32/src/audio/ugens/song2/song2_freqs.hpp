#pragma once

#include <iostream>

#include "src/audio/audio_util.hpp"

namespace Song2 {

const int FREQS_NOTES_SIZE = 12;

struct Freqs {
    float f[FREQS_NOTES_SIZE];
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

    float get(int i) {
        if (i < FREQS_NOTES_SIZE) {
            return f[i];
        } else {
            std::cout << "Freqs.get() out of range" << std::endl;
            return 0;
        }
    }

    void push(float _f) {
        if (size < FREQS_NOTES_SIZE) {
            f[size] = _f;
            ++size;
        } else {
            std::cout << "Freqs.push() beyond max capacity" << std::endl;
        }
    }

    void print() {
        std::cout << "Freqs:" << std::endl;
        for (int i = 0; i < size; i++) {
            std::cout << f[i] << " ";
        }
        std::cout << std::endl;
    }
};

struct Notes {
    int elts[FREQS_NOTES_SIZE];
    int size = 0;

    Notes() {}

    Notes(int i1) {
        push(i1);
    }

    Notes(int i1, int i2) {
        push(i1);
        push(i2);
    }

    Notes(int i1, int i2, int i3) {
        push(i1);
        push(i2);
        push(i3);
    }

    Notes(int i1, int i2, int i3, int i4) {
        push(i1);
        push(i2);
        push(i3);
        push(i4);
    }

    int get(int i) {
        if (i < FREQS_NOTES_SIZE) {
            return elts[i];
        } else {
            std::cout << "Notes.get() out of range" << std::endl;
            return 0;
        }
    }

    void push(int i) {
        if (size < FREQS_NOTES_SIZE) {
            elts[size] = i;
            ++size;
        } else {
            std::cout << "Notes.push() beyond max capacity" << std::endl;
        }
    }

    Freqs toFreqs() {
        Freqs freqs;
        for (int i = 0; i < size; i++) {
            freqs.push(
                noteToFreq(get(i))
            );
        }
        return freqs;
    }

    void print() {
        std::cout << "Notes:" << std::endl;
        for (int i = 0; i < size; i++) {
            std::cout << elts[i] << " ";
        }
        std::cout << std::endl;
    }
};

}
