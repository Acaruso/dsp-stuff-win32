#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"

const int _byte = 8;
const int _kb = 1024 * _byte;
const int _mb = 1024 * _kb;

class BufferAllocator {
public:
    int nextIdx = 0;
    std::vector<float>data = std::vector<float>((_mb * 20) / 32, 0.0f);
    int numAllocations = 0;

    int allocate() {
        int res = nextIdx;
        nextIdx += bufferSize;
        if (nextIdx >= data.size()) {
            std::cout << "BufferAllocator out of data!" << std::endl;
            return -1;
        } else {
            numAllocations++;
            // std::cout << "num allocations: " << numAllocations << std::endl;
            return res;
        }
    }

    void zeroAll() {
        std::fill(data.begin(), data.begin() + nextIdx, 0.0f);
    }
};

class UgenCtx {
public:
    BufferAllocator bufferAllocator;
};
