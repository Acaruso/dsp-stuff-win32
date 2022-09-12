#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/shared/shared_data.hpp"

inline void trigger(BaseUgen* ugen) {
    ugen->in[0][0] = 1.0;
}

class SampleMaker {
public:
    SharedData* sharedData = nullptr;
    UgenManager* root = nullptr;

    std::vector<bool> trigs = std::vector<bool>(8, false);

    unsigned samplesPerSecond = 0;
    double secondsPerSample = 0.0;

    double freq = 120.0;

    void init(SharedData* sharedData, unsigned long samplesPerSecond, double secondsPerSample) {
        this->sharedData = sharedData;
        this->root = &sharedData->rootUgen;
        this->samplesPerSecond = samplesPerSecond;
        this->secondsPerSample = secondsPerSample;
        initUgens();
    }

    void initUgens() {
        root->addUgen("osc", makeOscEnvFMUnisonRecorder(freq, secondsPerSample));
    }

    std::vector<double>& makeSamples(unsigned long sampleCounter) {
        BaseUgen* osc = root->getUgen("osc");

        if (trigs[0] == true) {
            trigs[0] = false;
            trigger(osc);
        }

        sharedData->rootUgen.run(sampleCounter);

        return osc->out[0];
    }
};
