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
#include "src/audio/ugens/splitter.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class SampleMaker {
public:
    SharedData* sharedData;
    UgenManager m;
    std::vector<bool> trigs = std::vector<bool>(8, false);

    unsigned samplesPerSecond = 0;
    double secondsPerSample = 0.0;

    int outputSink = 0;
    int envOnSink = 0;
    int mult = 0;
    int constValue = 0;
    int wtSinCarrier = 0;
    int wtSinMod = 0;
    int wtSinMod2 = 0;
    int ampEnv = 0;
    int ampVca = 0;
    int recorder = 0;
    int recorder2 = 0;

    int inner = 0;

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

    void initUgens() {
        int carrier = m.addUgen(makeOscEnv(freq, secondsPerSample));

        int mod = m.addUgen(makeOscEnv(freq * 0.5, secondsPerSample));

        int recorder = m.addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
        int recorder2 = m.addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

        // trigs
        m.connectIn(0, carrier, 0);
        m.connectIn(0, mod, 0);

        m.connect(mod, 0, carrier, 1);

        m.connectOut(carrier, 0, 0);
        m.connectOut(carrier, 1, 1);

        m.connect(carrier, 0, recorder, 0);
        m.connect(carrier, 1, recorder, 1);

        m.connect(carrier, 2, recorder2, 0);
        m.connect(carrier, 1, recorder2, 1);
    }

    double makeSample(unsigned long sampleCounter) {
        if (trigs[0] == true) {
            trigs[0] = false;
            m.in[0] = 1.0;
        } else {
            m.in[0] = 0.0;
        }

        m.run(getTime(sampleCounter));

        sharedData->sharedBuffers[0].active = (m.out[1] == 1.0);
        sharedData->sharedBuffers[1].active = (m.out[1] == 1.0);

        return m.out[0];
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
