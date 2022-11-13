#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// out[0] - audio signal

class AHRExpEnvScale : public BaseUgen {
public:
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

    AHRExpEnvScale(
        UgenCtx* _ugenCtx,
        AHRScaleData _ahrScaleData,
        float _level=1.0f
    ) {
        typeStr = "AHRExpEnvScale";
        ugenCtx = _ugenCtx;
        ahrScaleData = _ahrScaleData;
        setRatioOffset();
        level = _level;

        attackSamps  = mstosampsFloor1(ahrScaleData.a);
        holdSamps    = mstosampsFloor1(ahrScaleData.h);
        releaseSamps = mstosampsFloor1(ahrScaleData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta  = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;

        numIns = 1;
        numOuts = 1;
        allocateBuffers(typeStr);
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
        unsigned in0  = in[0];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (READ_IN(d, in0, i) == 1.0f) {
                trigger();
            }

            if (!on) {
                WRITE_OUT(d, out0, i, 0.0f);
            } else {
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

                ++timer;

                WRITE_OUT(
                    d,
                    out0,
                    i,
                    ((envSig * ratio) + offset) * level
                );
            }
        }
    }

    inline void trigger() {
        on = true;
        linearEnvSig = 0.0f;
        envSig = 0.0f;
        timer = 0;
    }
};
