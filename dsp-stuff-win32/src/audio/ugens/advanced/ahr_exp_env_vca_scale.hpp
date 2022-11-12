#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// in[1]  - audio signal
// out[0] - audio signal

class AHRExpEnvVcaScale : public BaseUgen {
public:
    AHRData ahrData;
    float low  = 0.0f;
    float high = 0.0f;
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

    AHRExpEnvVcaScale(
        UgenCtx* _ugenCtx,
        AHRData _ahrData,
        float _low,
        float _high,
        float _level=1.0f
    ) {
        typeStr = "AHRExpEnvVcaScale";
        ugenCtx = _ugenCtx;
        ahrData = _ahrData;
        level = _level;

        setScale(_low, _high);

        attackSamps  = mstosampsFloor1(ahrData.a);
        holdSamps    = mstosampsFloor1(ahrData.h);
        releaseSamps = mstosampsFloor1(ahrData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta  = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;

        numIns = 2;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void setScale(float _low, float _high) {
        low = _low;
        high = _high;
        ratio = high - low;
        offset = low;
    }

    void setScaleLow(float _low) {
        setScale(_low, high);
    }

    void setScaleHigh(float _high) {
        setScale(low, _high);
    }

    void setAhr(AHRData _ahrData) {
        ahrData = _ahrData;

        attackSamps = mstosampsFloor1(ahrData.a);
        holdSamps = mstosampsFloor1(ahrData.h);
        releaseSamps = mstosampsFloor1(ahrData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void setAttack(float _a) {
        ahrData.a = _a;

        attackSamps = mstosampsFloor1(ahrData.a);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
    }

    void setHold(float _h) {
        ahrData.h = _h;

        holdSamps = mstosamps(ahrData.h);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;
    }

    void setRelease(float _r) {
        ahrData.r = _r;

        releaseSamps = mstosampsFloor1(ahrData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        releaseDelta = 1.0f / (float)releaseSamps;
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;

        unsigned in0 = in[0];
        unsigned in1 = in[1];
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
                    ((envSig * ratio) + offset) * level * READ_IN(d, in1, i)
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
