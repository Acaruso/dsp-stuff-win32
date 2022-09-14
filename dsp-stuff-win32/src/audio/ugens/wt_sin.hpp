#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

class WTSin : public BaseUgen {
public:
    double freq = 0.0;
    double phase = 0.0;

    unsigned size = 1024;
    std::vector<double> wavetable;

    double dSize = size;
    double dSizexSecondsPerSample = 0.0;

    int i = 0;
    double frac = 0.0;
    double sig = 0.0;

    WTSin() {
        wavetable.resize(size + 1, 0.0);

        dSizexSecondsPerSample = dSize * secondsPerSample;

        double phase = 0.0;
        double delta = 1.0 / (double)size;

        // phase ranges from 0 to 1
        // multiply by twoPi to make it range from 0 to twoPi

        int i = 0;
        for (; i < size; ++i) {
            wavetable[i] = sin(phase * twoPi);
            phase += delta;
        }

        wavetable[i] = 0.0;
    }

    // in[0] - phase reset
    // in[1] - theta, used for phase modulation

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

            out[0][j] = sig;
        }
    }
};
