#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

// one-shot wavetable -- useful for envelopes

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class WavetableEnv : public BaseUgen {
public:
    std::vector<float>* wavetable;
    int timer = 0;
    int durationSamps = 0;
    int wtIdx = 0;
    float f_wtIdx = 0;
    float sig;
    float ratio = 0.0f;
    bool on = false;

    WavetableEnv(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float durationMs       // desired duration in ms
    ) {
        ugenCtx = _ugenCtx;
        wavetable = _wavetable;
        durationSamps = mstosamps(durationMs);

        // ratio = to / from
        // do wavetable size - 1 because lerp accesses wavetable[i] and wavetable[i + 1]
        ratio = ((float)wavetable->size() - 1) / (float)durationSamps;

        numIns = 1;
        numOuts = 2;
        allocateBuffers("WavetableEnv");
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned out0 = out[0];
        unsigned out1 = out[1];

        if (READ_IN(d, in0, 0) == 1.0f) {
            trigger();
        }

        if (!on) {
            fillBuffer(d, out0, bufferSize, 0.0f);
        } else {
            for (int i = 0; i < bufferSize; ++i) {
                if (timer < durationSamps) {
                    f_wtIdx = timer * ratio;
                    wtIdx = (int)f_wtIdx;
                    sig = LERP_WT((*wavetable), wtIdx, f_wtIdx);
                } else {
                    sig = 0.0f;
                    on = false;
                }

                ++timer;

                WRITE_OUT(d, out0, i, sig);
            }
        }

        WRITE_OUT(d, out1, 0, on ? 1.0f : 0.0f);
    }

    inline void trigger() {
        on = true;
        sig = 0.0f;
        timer = 0;
    }
};
