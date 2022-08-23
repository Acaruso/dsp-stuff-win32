#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
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

    unsigned samplesPerSecond = 0;
    double secondsPerSample = 0.0;

    int sink = 0;
    int envOnSink = 0;
    int splitter = 0;
    int envOnSplitter = 0;
    int wtSinCarrier = 0;
    int wtSinMod = 0;
    int ampEnv = 0;
    int modEnv = 0;
    int recorder = 0;

    double r = 0.0;
    double freq = 120.0;
    double ampA = 1;
    double ampH = 200;
    double ampR = 500;

    unsigned sharedBufferIdx = 0;

    void init(SharedData* sharedData, unsigned long samplesPerSecond, double secondsPerSample) {
        this->sharedData = sharedData;
        this->samplesPerSecond = samplesPerSecond;
        this->secondsPerSample = secondsPerSample;
        initUgens();
        unsigned ampSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);
        sharedData->sampleBuffer.resize(ampSamps, 0.0);
    }

    void initUgens() {
        sink = m.addUgen(new Sink());
        envOnSink = m.addUgen(new Sink());

        splitter = m.addUgen(new Splitter(4));
        envOnSplitter = m.addUgen(new Splitter(4));

        wtSinMod = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinMod))->freq = freq / 2.0;

        wtSinCarrier = m.addUgen(new WTSin(secondsPerSample));
        ((WTSin*)m.getUgen(wtSinCarrier))->freq = freq;

        ampEnv = m.addUgen(new AHREnv(ampA, ampH, ampR));

        recorder = m.addUgen(new Recorder(&sharedData->sampleBuffer));

        m.addConnection(wtSinMod, 0, wtSinCarrier, 0);

        m.addConnection(wtSinCarrier, 0, ampEnv, 0);

        m.addConnection(ampEnv, 0, splitter, 0);

        m.addConnection(splitter, 0, sink, 0);

        m.addConnection(splitter, 1, recorder, 0);

        m.addConnection(ampEnv, 1, envOnSplitter, 0);

        m.addConnection(envOnSplitter, 0, recorder, 1);

        m.addConnection(envOnSplitter, 1, envOnSink, 0);
    }

    double makeSample(unsigned long sampleCounter, std::string& message) {
        double t = getTime(sampleCounter);

        m.zeroAllInSigs();
        
        if (message == "trig") {
            m.getUgen(ampEnv)->inSigs[1] = 1.0;
            sharedBufferIdx = 0;
        } else {
            m.getUgen(ampEnv)->inSigs[1] = 0.0;
        }

        m.runAll(t);

        sharedData->envOn = (m.getUgen(envOnSink)->inSigs[0] == 1.0);

        return (m.getUgen(sink)->inSigs[0] * 1.0);
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
