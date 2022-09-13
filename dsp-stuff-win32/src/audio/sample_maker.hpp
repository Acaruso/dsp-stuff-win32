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

    std::vector<BaseUgen*> toTrigger = std::vector<BaseUgen*>(64, nullptr);
    int toTriggerSize = 0;

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
        int osc = root->addUgen("osc", makeOscEnvFMUnisonRecorder(freq, secondsPerSample));
        root->connectOut(osc, 0, 0);
    }

    std::vector<double>& makeSamples(unsigned long sampleCounter) {
        for (int i = 0; i < toTriggerSize; ++i) {
            trigger(toTrigger[i]);
        }

        sharedData->rootUgen.run(sampleCounter);

        return root->out[0];
    }
};
