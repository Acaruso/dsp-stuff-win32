#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

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
        int osc = root->addUgen(makeOscEnvFMUnison(freq, secondsPerSample));
        
        root->addName("osc", osc);

        int recorder1 = root->addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
        int recorder2 = root->addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

        root->connect(osc, 0, recorder1, 0);
        root->connect(osc, 2, recorder1, 1);

        root->connect(osc, 1, recorder2, 0);
        root->connect(osc, 2, recorder2, 1);
    }

    std::vector<double>& makeSamples(unsigned long sampleCounter) {
        BaseUgen* osc = root->getUgen("osc");

        if (trigs[0] == true) {
            trigs[0] = false;
            osc->in[0][0] = 1.0;
        } 

        sharedData->rootUgen.run(sampleCounter);

        sharedData->sharedBuffers[0].active = (osc->out[2][0] == 1.0);
        sharedData->sharedBuffers[1].active = (osc->out[2][0] == 1.0);

        return osc->out[0];
    }
};
