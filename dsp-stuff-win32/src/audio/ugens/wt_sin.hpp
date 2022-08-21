#pragma once

#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/shared/shared_constants.hpp"

class WTSin {
public:
    AHREnv env;
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

    void trigger(
        double a,
        double h,
        double r,
        double freq
    ) {
        this->freq = freq;
        env.trigger(a, h, r);
    }

    double get(double t);
    double get(double theta, double t);
};
