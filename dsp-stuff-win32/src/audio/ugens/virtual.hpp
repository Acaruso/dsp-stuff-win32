#pragma once

#include <string>
#include <unordered_map>

#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/pass.hpp"

class Virtual : public BaseUgen {
public:
    std::unordered_map<std::string, BaseUgen*> ugens;
    BaseUgen* pVIn = nullptr;
    BaseUgen* pVOut = nullptr;
    int vIn = 0;
    int vOut = 0;

    Virtual(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 0;
        numOuts = 0;
        isVirtual = true;
        allocateBuffers("Virtual");
    }

    Virtual(
        UgenCtx* _ugenCtx,
        int _vIn,
        int _vOut,
        BaseUgen* _pVIn,
        BaseUgen* _pVOut
    ) {
        ugenCtx = _ugenCtx;
        vIn = _vIn;
        vOut = _vOut;
        pVIn = _pVIn;
        pVOut = _pVOut;
        numIns = 0;
        numOuts = 0;
        isVirtual = true;
        allocateBuffers("Virtual");
    }

    void addName(std::string name, BaseUgen* ugen) {
        ugens[name] = ugen;
    }

    BaseUgen* getUgen(std::string name) {
        return ugens[name];
    }

    void writeIn(int inIdx, int sampleIdx, float sample) override {
        int inOffset = pVIn->in[inIdx];
        ugenCtx->bufferAllocator.data[inOffset + sampleIdx] += sample;
    }

    void run(unsigned sampleCounter) override {
        // do nothing
    }
};
