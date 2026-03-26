#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// in[1]  - phase mod
// in[2]  - frequency
// out[0] - audio signal

class WavetableOp : public BaseUgen {
public:
    // wavetable
    float freq = 100.0f;
    float phase = 0.0f;

    std::vector<float>* wavetable;
    int size = 0;
    float fSize = 0.0f;
    float fSizexSecondsPerSample = 0.0f;

    int wtIdx = 0;
    float wtSig = 0.0f;

    // env
    AHRScaleData ahrScaleData;
    float ratio = 0.0f;
    float offset = 0.0f;

    unsigned attackSamps = 0;
    unsigned holdSamps = 0;
    unsigned releaseSamps = 0;

    unsigned attackHoldSamps = 0;
    unsigned attackHoldReleaseSamps = 0;

    float attackDelta = 0.0f;
    float releaseDelta = 0.0f;

    bool on = false;
    float linearEnvSig = 0.0f;
    float envSig = 0.0f;
    unsigned timer = 0;

    WavetableOp(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        AHRScaleData _ahrScaleData,
        float _level=1.0f
    ) {
        // common
        typeStr = "WavetableOp";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 3;
        numOuts = 1;
        allocateBuffers(typeStr);

        // wavetable
        wavetable = _wavetable;

        size = wavetable->size() - 1;
        fSize = (float)size;
        fSizexSecondsPerSample = fSize * secondsPerSample;

        // env
        ahrScaleData = _ahrScaleData;
        setRatioOffset();

        attackSamps  = mstosampsFloor1(ahrScaleData.a);
        holdSamps    = mstosampsFloor1(ahrScaleData.h);
        releaseSamps = mstosampsFloor1(ahrScaleData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta  = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void setScale(float low, float high) {
        ahrScaleData.low = low;
        ahrScaleData.high = high;
        setRatioOffset();
    }

    void setScaleLow(float _low) {
        ahrScaleData.low = _low;
        setRatioOffset();
    }

    void setScaleHigh(float _high) {
        ahrScaleData.high = _high;
        setRatioOffset();
    }

    void setRatioOffset() {
        ratio = ahrScaleData.high - ahrScaleData.low;
        offset = ahrScaleData.low;
    }

    void setAhr(AHRScaleData _ahrScaleData) {
        ahrScaleData = _ahrScaleData;
        setRatioOffset();

        attackSamps = mstosampsFloor1(ahrScaleData.a);
        holdSamps = mstosampsFloor1(ahrScaleData.h);
        releaseSamps = mstosampsFloor1(ahrScaleData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void setAttack(float _a) {
        ahrScaleData.a = _a;

        attackSamps = mstosampsFloor1(ahrScaleData.a);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
    }

    void setHold(float _h) {
        ahrScaleData.h = _h;

        holdSamps = mstosamps(ahrScaleData.h);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;
    }

    void setRelease(float _r) {
        ahrScaleData.r = _r;

        releaseSamps = mstosampsFloor1(ahrScaleData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned in1 = in[1];
        unsigned in2 = in[2];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (READ_IN(d, in0, i) == 1.0f) {
                trigger();
            }

            if (!on) {
                WRITE_OUT(d, out0, i, 0.0f);
            } else {
                // wavetable //////////////////////////////////////////////////

                freq = READ_IN(d, in2, i);

                wtIdx = (int)phase;

                wtSig = LERP_WT((*wavetable), wtIdx, phase);

                // get next phase
                phase += (fSizexSecondsPerSample * freq) + READ_IN(d, in1, i);

                // phase = phase % wavetable size
                while (phase >= fSize) {
                    phase -= fSize;
                }

                while (phase < 0) {
                    phase += fSize;
                }

                // env ////////////////////////////////////////////////////////

                if (timer < attackSamps) {
                    linearEnvSig += attackDelta;
                    envSig = sqrt(linearEnvSig);
                } else if (timer < attackHoldSamps) {
                    envSig = 1.0f;
                } else if (timer < attackHoldReleaseSamps) {
                    linearEnvSig -= releaseDelta;
                    envSig = linearEnvSig * linearEnvSig; 
                } else if (timer >= attackHoldReleaseSamps) {
                    envSig = 0.0f;
                    on = false;
                }

                envSig = (envSig * ratio) + offset;

                ++timer;

                WRITE_OUT(
                    d,
                    out0,
                    i,
                    wtSig * envSig * level
                );
            }
        }
    }

    inline void trigger() {
        // wavetable
        phase = 0.0f;

        // env
        on = true;
        linearEnvSig = 0.0f;
        envSig = 0.0f;
        timer = 0;
    }
};
