#pragma once

#include <cmath>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

namespace Song2 {

class Env : public BaseGen {
public:
    float level = 1.0f;

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

    float low = 0.0f;
    float high = 1.0f;

    Env() {}

    Env(AHRData _ahrData, float _level=1.0f) {
        ahrData = _ahrData;
        level = _level;

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

    void setLow(float f) {
        low = f;
    }

    void setHigh(float f) {
        high = f;
    }

    void trigger() {
        on = true;
        linearSig = 0.0f;
        sig = 0.0f;
        timer = 0;
    }

    float get() {
        if (!on) {
            return 0.0f;
        } else {
            // return sig * level;
            // return ((sig + low) * high) * level;
            return (sig * (high - low)) + low;
        }
    }

    void run() override {
        if (on) {
            if (timer < attackSamps) {
                linearSig += attackDelta;
                sig = sqrt(linearSig);
            } else if (timer < attackHoldSamps) {
                sig = 1.0f;
            } else if (timer < attackHoldReleaseSamps) {
                linearSig -= releaseDelta;
                sig = linearSig * linearSig;
            } else if (timer >= attackHoldReleaseSamps) {
                sig = 0.0f;
                on = false;
            }
        }

        ++timer;
    }
};

}
