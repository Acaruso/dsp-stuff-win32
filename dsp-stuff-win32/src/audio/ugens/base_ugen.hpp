#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/main/util.hpp"

#define READ_IN(data, offset, sampleIdx) data[offset + sampleIdx]

#define WRITE_OUT(data, offset, sampleIdx, sample) data[offset + sampleIdx] = sample

class BaseUgen {
public:
    int numIns = 0;
    int numOuts = 0;

    std::vector<unsigned> in;
    std::vector<unsigned> out;

    std::vector<bool> inActive;
    std::vector<bool> outActive;

    UgenCtx* ugenCtx = nullptr;

    std::string typeStr = "BaseUgen";

    float level = 1.0f;

    virtual void allocateBuffers(std::string str="") {
        resizeIns(numIns, str);
        resizeOuts(numOuts, str);
    }

    void resizeIns(int newSize, std::string str) {
        for (int i = 0; i < newSize; i++) {
            unsigned newOffset = ugenCtx->bufferAllocator.allocate(str);
            in.push_back(newOffset);
        }

        inActive.resize(newSize, false);
    }

    void resizeOuts(int newSize, std::string str) {
        out.resize(newSize, 0);
        outActive.resize(newSize, false);
    }

    void addIn() {
        ++numIns;
        unsigned newOffset = ugenCtx->bufferAllocator.allocate();
        in.push_back(newOffset);
        inActive.push_back(false);
    }

    void assertInInactive(int port) {
        if (getInActive(port)) {
            std::cout << typeStr << ".in[" << port << "] is already connected!";
            exit(1);
        }
    }

    void assertOutInactive(int port) {
        if (getOutActive(port)) {
            std::cout << typeStr << ".out[" << port << "] is already connected!";
            exit(1);
        }
    }

    bool getInActive(int port) {
        return inActive[port];
    }

    bool getOutActive(int port) {
        return outActive[port];
    }

    void setInActive(int port, bool val) {
        inActive[port] = val;
    }

    void setOutActive(int port, bool val) {
        outActive[port] = val;
    }

    void setLevel(float newLevel) {
        // do we actually want to clamp this?
        // level = clamp(newLevel, 0.0f, 1.0f);
        level = newLevel;
    }

    void zeroIns() {
        for (auto offset : in) {
            std::fill(
                ugenCtx->bufferAllocator.data.begin() + offset,
                ugenCtx->bufferAllocator.data.begin() + offset + bufferSize,
                0.0f
            );
        }
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
