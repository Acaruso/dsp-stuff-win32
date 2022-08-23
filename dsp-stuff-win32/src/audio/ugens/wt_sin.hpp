#pragma once

#include <iostream>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_constants.hpp"

class WTSin : public BaseUgen {
public:
    double freq = 0.0;
    double secondsPerSample = 0.0;
    double phase = 0.0;

    unsigned size = 1024;
    std::vector<double> wavetable;

    WTSin() {
        wavetable.resize(size + 1, 0.0);

        double phase = 0.0;
        double delta = 1.0 / (double)wavetable.size();

        // phase ranges from 0 to 1
        // multiply by twoPi to make it range from 0 to twoPi

        int i = 0;
        for (; i < size; ++i) {
            wavetable[i] = sin(phase * twoPi);
            phase += delta;
        }

        wavetable[i] = 0.0;
    }

    WTSin(double _secondsPerSample) {
        secondsPerSample = _secondsPerSample;
        wavetable.resize(size + 1, 0.0);

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

    void _run(double t) override {
        double theta = inSigs[0];

        double dSize = (double)size;

        int i = (int)phase;
        double frac = phase - i;

        // linear interpolation
        double sig = wavetable[i] + (frac * (wavetable[i + 1] - wavetable[i]));

        // get next phase
        phase += (dSize * freq * secondsPerSample) + theta;

        // phase = phase % wavetable size
        while (phase >= dSize) {
            phase -= dSize;
        }

        while (phase < 0) {
            phase += dSize;
        }

        outSigs[0] = sig;
    }
};
