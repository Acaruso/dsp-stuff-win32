#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
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
        outputSink = m.addUgen(new Sink());
        envOnSink = m.addUgen(new Sink());

        mult = m.addUgen(new Mult());
        constValue = m.addUgen(new ConstValue(8));

        wtSinMod = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinMod))->freq = freq / 2.0;

        wtSinMod2 = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinMod2))->freq = freq * 4;

        wtSinCarrier = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinCarrier))->freq = freq;

        ampEnv = m.addUgen(new AHREnv(ampA, ampH, ampR));
        ampVca = m.addUgen(new Mult());

        recorder  = m.addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
        recorder2 = m.addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

        // route mod oscs into carrier osc
        m.connect(wtSinMod, 0, mult, 0);
        m.connect(wtSinMod2, 0, mult, 0);
        m.connect(constValue, 0, mult, 1);
        m.connect(mult, 0, wtSinCarrier, 0);

        // route ampEnv and wtSinCarrier to ampVca
        m.connect(ampEnv, 0, ampVca, 0);
        m.connect(wtSinCarrier, 0, ampVca, 1);

        // recorder.in[0] -- input
        // recorder.in[1] -- on/off
        m.connect(ampVca, 0, recorder, 0);
        m.connect(ampEnv, 1, recorder, 1);

        m.connect(ampEnv, 0, recorder2, 0);
        m.connect(ampEnv, 1, recorder2, 1);

        m.connect(ampEnv, 1, envOnSink, 0);

        // route ampVca to output
        m.connect(ampVca, 0, outputSink, 0);
    }

    double makeSample(unsigned long sampleCounter) {
        double t = getTime(sampleCounter);

        m.zeroIns();

        if (trigs[0] == true) {
            trigs[0] = false;
            m.getUgen(ampEnv)->in[0] = 1.0;
        } else {
            m.getUgen(ampEnv)->in[0] = 0.0;
        }

        m.run(t);

        bool active = (m.getUgen(envOnSink)->in[0] == 1.0);
        sharedData->sharedBuffers[0].active = active;
        sharedData->sharedBuffers[1].active = active;

        double outSig = m.getUgen(outputSink)->in[0] * 1.0;

        return outSig;
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
