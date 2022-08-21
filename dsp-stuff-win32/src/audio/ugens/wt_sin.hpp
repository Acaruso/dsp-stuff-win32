#pragma once

#include <iostream>
#include <vector>

#include "src/shared/shared_constants.hpp"

class WTSin {
public:
    double freq;
    double secondsPerSample;
    double phase{0};

    unsigned size{1024};
    std::vector<double> wavetable;

    WTSin() {
        double phase = 0.0;
        double delta = 1.0 / (double)size;

        // phase ranges from 0 to 1
        // multiply by twoPi to make it range from 0 to twoPi

        for (unsigned i = 0; i < size; i++) {
            wavetable.push_back(sin(phase * twoPi));
            phase += delta;
        }

        wavetable.push_back(0.0);
    }

    WTSin(double secondsPerSample_) {
        secondsPerSample = secondsPerSample_;
        double phase = 0.0;
        double delta = 1.0 / (double)size;

        for (unsigned i = 0; i < size; i++) {
            wavetable.push_back(sin(phase * twoPi));
            phase += delta;
        }

        wavetable.push_back(0.0);
    }

    double get(double t) {
        return get(0, t);
    }

    double get(double theta, double t) {
        int i = (int)phase;
        double frac = phase - i;

        // linear interpolation
        double sig = wavetable[i] + (frac * (wavetable[i + 1] - wavetable[i]));

        // get next phase
        phase += ((double)size * freq * secondsPerSample) + theta;

        // phase = phase % wavetable size
        double dSize = (double)size;

        while (phase >= dSize) {
            phase -= dSize;
        }

        while (phase < 0) {
            phase += dSize;
        }

        return sig;
    }
};
