#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/sink.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class SampleMaker {
public:
    SharedData* sharedData = nullptr;
    UgenManager* m = nullptr;
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
        this->samplesPerSecond = samplesPerSecond;
        this->secondsPerSample = secondsPerSample;
        initUgens();
        unsigned ampSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);
        sharedData->sharedBuffers[0].data.resize(ampSamps, 0.0);
        sharedData->sharedBuffers[1].data.resize(ampSamps, 0.0);
    }

    // simple:
    // void initUgens() {
    //     int recorder1 = m.addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
    //     int recorder2 = m.addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

    //     int osc1 = m.addUgen(makeOscEnv(freq, secondsPerSample));
    //     m.connectIn(0, osc1, 0);

    //     m.connect(osc1, 0, recorder1, 0);
    //     m.connect(osc1, 2, recorder1, 1);

    //     m.connect(osc1, 1, recorder2, 0);
    //     m.connect(osc1, 2, recorder2, 1);

    //     m.connectOut(osc1, 0, 0);
    //     m.connectOut(osc1, 2, 1);
    // }

    // medium complexity:
    // void initUgens() {
    //     int osc1 = m.addUgen(makeOscEnvFM(freq, secondsPerSample));

    //     m.connectIn(0, osc1, 0);

    //     int mult = m.addUgen(new Mult());
    //     int constValue = m.addUgen(new ConstValue(0.5));

    //     m.connect(osc1, 0, mult, 0);

    //     m.connect(constValue, 0, mult, 1);

    //     m.connectOut(mult, 0, 0);
    //     m.connectOut(osc1, 2, 1);
    // }

    // highest complexity:
    void initUgens() {
        m = &sharedData->rootUgenManager;

        int recorder1 = m->addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
        int recorder2 = m->addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

        int osc1 = m->addUgen(makeOscEnvFM(freq, secondsPerSample));
        int osc2 = m->addUgen(makeOscEnvFM(freq + 0.2, secondsPerSample));
        int osc3 = m->addUgen(makeOscEnvFM(freq - 0.2, secondsPerSample));
        int osc4 = m->addUgen(makeOscEnvFM(freq + 0.4, secondsPerSample));
        int osc5 = m->addUgen(makeOscEnvFM(freq - 0.4, secondsPerSample));

        m->connectIn(0, osc1, 0);
        m->connectIn(0, osc2, 0);
        m->connectIn(0, osc3, 0);
        m->connectIn(0, osc4, 0);
        m->connectIn(0, osc5, 0);

        int mult = m->addUgen(new Mult());
        int constValue = m->addUgen(new ConstValue(0.2));

        m->connect(osc1, 0, mult, 0);
        m->connect(osc2, 0, mult, 0);
        m->connect(osc3, 0, mult, 0);
        m->connect(osc4, 0, mult, 0);
        m->connect(osc5, 0, mult, 0);

        m->connect(constValue, 0, mult, 1);

        m->connect(mult, 0, recorder1, 0);
        m->connect(osc1, 2, recorder1, 1);

        m->connect(osc1, 1, recorder2, 0);
        m->connect(osc1, 2, recorder2, 1);

        m->connectOut(mult, 0, 0);
        m->connectOut(osc1, 2, 1);
    }

    std::vector<double>& makeSamples(unsigned long sampleCounter) {
        if (trigs[0] == true) {
            trigs[0] = false;
            m->in[0][0] = 1.0;
        } 

        m->run(sampleCounter);

        sharedData->sharedBuffers[0].active = (m->out[1][0] == 1.0);
        sharedData->sharedBuffers[1].active = (m->out[1][0] == 1.0);

        return m->out[0];
    }
};
