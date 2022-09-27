#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/shared/audio_buffer.hpp"

struct UgenConnection {
    int destId;
    int sourcePort;
    int destPort;

    bool operator==(const UgenConnection& other) const {
        return (
            destId == other.destId
            && sourcePort == other.sourcePort
            && destPort == other.destPort
        );
    }
};

class BaseUgen {
public:
    int numIns = 0;
    int numOuts = 0;

    std::vector<unsigned> in;                    // offsets into ugenCtx buffer data
    std::vector<std::vector<unsigned>> out;
    UgenCtx* ugenCtx = nullptr;

    std::vector<UgenConnection> connections;

    virtual void allocateBuffers() {
        resizeIns(numIns);
        resizeOuts(numOuts);
    }

    // override in UgenManager
    virtual void allocateBuffersRecursive() {
        in.clear();
        out.clear();
        resizeIns(numIns);
        resizeOuts(numOuts);
    }

    void resizeIns(int newSize) {
        for (int i = 0; i < newSize; i++) {
            unsigned newOffset = ugenCtx->bufferAllocator.allocate();
            in.push_back(newOffset);
        }
    }

    void resizeOuts(int newSize) {
        out.resize(newSize, std::vector<unsigned>());
    }

    inline float readIn(int inIdx, int sampleIdx) {
        return bufRead(in[inIdx], sampleIdx);
    }

    inline void writeOut(int outIdx, int sampleIdx, float sample) {
        for (auto _out : out[outIdx]) {
            bufWrite(_out, sampleIdx, sample);
        }
    }

    // write directly to input buffer
    // typically, don't need to use this
    inline void writeIn(int inIdx, int sampleIdx, float sample) {
        int inOffset = in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] += sample;
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

    void connect(int destId, int sourcePort, int destPort) {
        UgenConnection elt = { destId, sourcePort, destPort };

        if (std::find(connections.begin(), connections.end(), elt) == connections.end()) {
            connections.push_back(elt);
        }
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;

private:
    inline float bufRead(int offset, int i) {
        return ugenCtx->bufferAllocator.data[offset + i];
    }

    inline void bufWrite(int offset, int i, float sample) {
        ugenCtx->bufferAllocator.data[offset + i] += sample;
    }
};
