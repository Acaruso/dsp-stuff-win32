#pragma once

#include <iostream>
#include <vector>

#include "src/shared/shared_constants.hpp"

class WTSin {
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

    double get(double t) {
        return get(0, t);
    }

    double get(double theta, double t) {
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

        return sig;
    }
};
