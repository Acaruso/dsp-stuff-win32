#pragma once

#include <cmath>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

// one-shot wavetable -- useful for envelopes
// TODO: implement oscillating/continuously running wavetable

// in[0]  - trigger
// out[0] - envelope
// out[1] - on/off

class Wavetable : public BaseUgen {
public:
    std::vector<float>* wavetable;
    int timer = 0;
    int duration = 0;
    int wtIdx = 0;
    float sig;
    float ratio = 0.0f;
    bool on = false;

    Wavetable(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        int _duration       // desired duration in samples 
    ) {                     // (should this actually be in ms?)
        ugenCtx = _ugenCtx;
        wavetable = _wavetable;
        duration = _duration;

        // to compute ratio:
        // ratio = to / from
        ratio = (float)wavetable->size() / (float)duration;
        numIns = 2;
        numOuts = 1;
        allocateBuffers("Wavetable");
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
                // if (timer < wavetable->size()) {
                if (timer < duration) {
                    wtIdx = timer * ratio;
                    sig = (*wavetable)[wtIdx];
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
