#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
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

    int sink = 0;
    int envOnSink = 0;
    int splitter = 0;
    int envOnSplitter = 0;
    int mult = 0;
    int wtSinCarrier = 0;
    int wtSinMod = 0;
    int wtSinMod2 = 0;
    int ampEnv = 0;
    int modEnv = 0;
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
        sink = m.addUgen(new Sink());
        envOnSink = m.addUgen(new Sink());

        splitter = m.addUgen(new Splitter(4));

        envOnSplitter = m.addUgen(new Splitter(4));
        mult = m.addUgen(new Mult(8));

        wtSinMod = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinMod))->freq = freq / 2.0;

        wtSinMod2 = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinMod2))->freq = freq * 4;

        wtSinCarrier = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinCarrier))->freq = freq;

        ampEnv = m.addUgen(new AHREnv(ampA, ampH, ampR));

        recorder  = m.addUgen(new Recorder(&sharedData->sharedBuffers[0].data));
        recorder2 = m.addUgen(new Recorder(&sharedData->sharedBuffers[1].data));

        m.addConnection(wtSinMod, 0, mult, 0);
        m.addConnection(wtSinMod2, 0, mult, 0);
        m.addConnection(mult, 0, wtSinCarrier, 0);
        m.addConnection(wtSinCarrier, 0, ampEnv, 0);
        m.addConnection(ampEnv, 0, sink, 0);

        m.addConnection(ampEnv, 0, recorder, 0);
        m.addConnection(ampEnv, 1, recorder, 1);

        m.addConnection(ampEnv, 2, recorder2, 0);
        m.addConnection(ampEnv, 1, recorder2, 1);

        m.addConnection(ampEnv, 1, envOnSink, 0);
    }

    double makeSample(unsigned long sampleCounter) {
        double t = getTime(sampleCounter);

        m.zeroAllInSigs();

        if (trigs[0] == true) {
            trigs[0] = false;
            m.getUgen(ampEnv)->inSigs[1] = 1.0;
        } else {
            m.getUgen(ampEnv)->inSigs[1] = 0.0;
        }

        m.runAll(t);

        bool active = (m.getUgen(envOnSink)->inSigs[0] == 1.0);
        sharedData->sharedBuffers[0].active = active;
        sharedData->sharedBuffers[1].active = active;

        double outSig = m.getUgen(sink)->inSigs[0] * 1.0;

        return outSig;
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
