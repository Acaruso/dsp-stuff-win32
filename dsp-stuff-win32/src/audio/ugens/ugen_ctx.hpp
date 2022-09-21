#pragma once

#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"

const int _byte = 8;
const int _kb = 1024 * _byte;
const int _mb = 1024 * _kb;

class BufferAllocator {
    int nextIdx = 0;
    std::vector<float>data = std::vector<float>(_mb / 32, 0.0f);

    int allocate() {
        int res = nextIdx;
        ++nextIdx;
        if (nextIdx >= data.size()) {
            std::cout << "BufferAllocator out of data!" << std::endl;
            return -1;
        } else {
            return res;
        }
    }
};

class UgenCtx {
public:
    BufferAllocator bufferAllocator;
};
