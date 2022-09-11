#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

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

    double r = 0.0;
    double freq = 120.0;
    double ampA = 1;
    double ampH = 200;
    double ampR = 500;

    void init(SharedData* sharedData, unsigned long samplesPerSecond, double secondsPerSample) {
        this->sharedData = sharedData;
        this->root = &sharedData->rootUgen;
        this->samplesPerSecond = samplesPerSecond;
        this->secondsPerSample = secondsPerSample;
        initUgens();
        unsigned ampSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);
        sharedData->sharedBuffers[0].data.resize(ampSamps, 0.0);
        sharedData->sharedBuffers[1].data.resize(ampSamps, 0.0);
    }

    void initUgens() {
        int osc = root->addUgen(
            makeOscEnvFMUnisonRecorder(
                freq, 
                secondsPerSample,
                &sharedData->sharedBuffers[0].data,
                &sharedData->sharedBuffers[1].data
            )
        );
        
        root->addName("osc", osc);
    }

    std::vector<double>& makeSamples(unsigned long sampleCounter) {
        BaseUgen* osc = root->getUgen("osc");

        if (trigs[0] == true) {
            trigs[0] = false;
            trigger(osc);
        }

        sharedData->rootUgen.run(sampleCounter);

        sharedData->sharedBuffers[0].active = (osc->out[2][0] == 1.0);
        sharedData->sharedBuffers[1].active = (osc->out[2][0] == 1.0);

        return osc->out[0];
    }
};
