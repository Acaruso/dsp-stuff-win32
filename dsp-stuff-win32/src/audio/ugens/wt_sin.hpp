#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"
#include "src/shared/shared_constants.hpp"

// in[0]  - phase reset
// in[1]  - theta, used for phase modulation
// out[0] - output signal

class WTSin : public BaseUgen {
public:
    float freq = 0.0;
    float phase = 0.0;

    int size = 1024;
    AudioBuffer wavetable;

    float dSize = size;
    float dSizexSecondsPerSample = 0.0;

    int i = 0;
    float frac = 0.0;
    float sig = 0.0;

    WTSin() {
        resizeIns(2);
        resizeOuts(1);

        wavetable.resize(size + 1, 0.0);

        dSizexSecondsPerSample = dSize * secondsPerSample;

        float phase = 0.0f;
        float delta = 1.0f / (float)size;

        // phase ranges from 0 to 1
        // multiply by twoPi to make it range from 0 to twoPi

        int i = 0;
        for (; i < size; ++i) {
            wavetable[i] = (float)sin(phase * twoPi);
            phase += delta;
        }

        wavetable[i] = 0.0f;
    }

    void run(unsigned sampleCounter) override {
        if (in[0][0] == 1.0) {
            phase = 0.0;
        }

        for (int j = 0; j < bufferSize; ++j) {
            i = (int)phase;

            // linear interpolation:
            frac = phase - i;
            sig = wavetable[i] + (frac * (wavetable[i + 1] - wavetable[i]));

            // no interpolation:
            // sig = wavetable[i];

            // get next phase
            phase += (dSizexSecondsPerSample * freq) + in[1][j];   // in[1] == theta

            // phase = phase % wavetable size
            while (phase >= dSize) {
                phase -= dSize;
            }

            while (phase < 0) {
                phase += dSize;
            }

            writeOut(0, j, sig);
        }
    }
};
