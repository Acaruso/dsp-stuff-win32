#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/audio/ugens/wavetable_utils.hpp"

const int _byte = 8;
const int _kb = 1024 * _byte;
const int _mb = 1024 * _kb;

class BufferAllocator {
public:
    int nextIdx = 0;
    int numAllocations = 0;
    std::vector<float> data = std::vector<float>((_mb * 20) / 32, 0.0f);
    std::unordered_map<std::string, int> allocationMap;

    int allocate(std::string str="") {
        if (str != "") {
            allocationMap[str] += 1;
        }
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

    void printAllocationMap() {
        std::cout << "num allocations: " << numAllocations << std::endl;
        for (auto& [k, v] : allocationMap) {
            std::cout << k << " : " << v << std::endl;
        }
        std::cout << std::endl;
    }
};

class Wavetables {
public:
    std::vector<float>* sin;
    std::vector<float>* tanh;
    std::vector<float>* noise;

    Wavetables() {
        sin = makeSinWavetable(1024);
        tanh = makeTanhWavetable(1024, 3.0);
        noise = makeWhiteNoiseWavetable(1024);
    }
};

class Waves {
public:
    std::vector<float> wav1;
};

class UgenCtx {
public:
    BufferAllocator bufferAllocator;
    Wavetables wavetables;
    Waves waves;
};
