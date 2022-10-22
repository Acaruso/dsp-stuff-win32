#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - phase reset
// in[1]  - theta, used for phase modulation
// out[0] - output signal

class WavetableOsc : public BaseUgen {
public:
    float freq = 0.0f;
    float phase = 0.0f;

    int size = 0;
    std::vector<float>* wavetable;

    float fSize = 0.0f;
    float fSizexSecondsPerSample = 0.0f;

    int wtIdx = 0;
    float sig = 0.0f;

    WavetableOsc(
        UgenCtx* _ugenCtx, 
        std::vector<float>* _wavetable, 
        float _freq
    ) {
        typeStr = "WavetableOsc";
        ugenCtx = _ugenCtx;
        wavetable = _wavetable;
        freq = _freq;
        
        size = wavetable->size() - 1;
        fSize = (float)size;
        fSizexSecondsPerSample = fSize * secondsPerSample;

        numIns = 2;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned in0 = in[0];
        unsigned in1 = in[1];
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (READ_IN(d, in0, i) == 1.0f) {
                phase = 0.0f;
            }

            wtIdx = (int)phase;

            sig = LERP_WT((*wavetable), wtIdx, phase);

            // get next phase
            phase += (fSizexSecondsPerSample * freq) + READ_IN(d, in1, i);   // in[1] == theta

            // phase = phase % wavetable size
            while (phase >= fSize) {
                phase -= fSize;
            }

            while (phase < 0) {
                phase += fSize;
            }

            WRITE_OUT(d, out0, i, sig * level);
        }
    }
};
