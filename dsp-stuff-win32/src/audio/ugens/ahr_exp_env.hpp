#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class AHRExpEnv : public BaseUgen {
public:
    AHRData ahrData;

    unsigned attackSamps = 0;
    unsigned holdSamps = 0;
    unsigned releaseSamps = 0;

    unsigned attackHoldSamps = 0;
    unsigned attackHoldReleaseSamps = 0;

    float attackDelta = 0.0f;
    float releaseDelta = 0.0f;

    bool on = false;
    float linearSig = 0.0f;
    float sig = 0.0f;
    unsigned timer = 0;

    AHRExpEnv(UgenCtx* _ugenCtx, AHRData _ahrData, float _level=1.0f) {
        typeStr = "AHRExpEnv";
        ugenCtx = _ugenCtx;
        ahrData = _ahrData;
        level = _level;

        numIns = 1;
        numOuts = 2;
        allocateBuffers(typeStr);

        attackSamps = mstosampsFloor1(ahrData.a);
        holdSamps = mstosampsFloor1(ahrData.h);
        releaseSamps = mstosampsFloor1(ahrData.r);

        attackHoldSamps = attackSamps + holdSamps;
        attackHoldReleaseSamps = attackSamps + holdSamps + releaseSamps;

        attackDelta = 1.0f / (float)attackSamps;
        releaseDelta = 1.0f / (float)releaseSamps;
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
        unsigned out0 = out[0];
        unsigned out1 = out[1];

        for (int i = 0; i < bufferSize; ++i) {
            if (READ_IN(d, in0, i) == 1.0f) {
                trigger();
            }

            if (!on) {
                WRITE_OUT(d, out0, i, 0.0f);
                WRITE_OUT(d, out1, i, 0.0f);
            } else {
                if (timer < attackSamps) {
                    linearSig += attackDelta;
                    sig = sqrt(linearSig);
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldSamps) {
                    sig = 1.0f;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer < attackHoldReleaseSamps) {
                    linearSig -= releaseDelta;
                    sig = linearSig * linearSig;
                    WRITE_OUT(d, out1, i, 1.0f);
                } else if (timer >= attackHoldReleaseSamps) {
                    sig = 0.0f;
                    on = false;
                    WRITE_OUT(d, out1, i, 0.0f);
                }

                ++timer;

                WRITE_OUT(d, out0, i, sig * level);
            }
        }
    }

    inline void trigger() {
        on = true;
        linearSig = 0.0f;
        sig = 0.0f;
        timer = 0;
    }
};
