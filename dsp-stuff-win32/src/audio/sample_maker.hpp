#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/sink.hpp"
#include "src/audio/ugens/splitter.hpp"
#include "src/audio/ugens/wt_sin.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class SampleMaker {
public:
    SharedData* sharedData;
    unsigned samplesPerSecond = 0;
    double secondsPerSample = 0.0;

    BaseUgen* sink = nullptr;
    BaseUgen* envOnSink = nullptr;

    BaseUgen* splitter = nullptr;
    BaseUgen* envOnSplitter = nullptr;

    BaseUgen* wtSinCarrier = nullptr;
    BaseUgen* wtSinMod = nullptr;
    BaseUgen* ampEnv = nullptr;
    BaseUgen* modEnv = nullptr;

    BaseUgen* recorder = nullptr;

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
        sink = new Sink();
        envOnSink = new Sink();

        splitter = new Splitter();
        envOnSplitter = new Splitter();

        wtSinMod = new WTSin(secondsPerSample);
        ((WTSin*)wtSinMod)->freq = freq / 2.0;

        wtSinCarrier = new WTSin(secondsPerSample);
        ((WTSin*)wtSinCarrier)->freq = freq;

        ampEnv = new AHREnv(ampA, ampH, ampR);

        recorder = new Recorder(&sharedData->sampleBuffer);

        wtSinMod->addOutput(wtSinCarrier, 0, 0);

        wtSinCarrier->addOutput(ampEnv, 0, 0);

        ampEnv->addOutput(splitter, 0, 0);

        splitter->addOutput(sink, 0, 0);

        splitter->addOutput(recorder, 1, 0);

        ampEnv->addOutput(envOnSplitter, 1, 0);
        envOnSplitter->addOutput(recorder, 0, 1);
        envOnSplitter->addOutput(envOnSink, 1, 0);
    }

    double makeSample(unsigned long sampleCounter, std::string& message) {
        double t = getTime(sampleCounter);

        if (message == "trig") {
            ampEnv->inputs[1] = 1.0;
            sharedBufferIdx = 0;
        } else {
            ampEnv->inputs[1] = 0.0;
        }

        wtSinMod->get(t);
        wtSinCarrier->get(t);
        ampEnv->get(t);
        splitter->get(t);
        envOnSplitter->get(t);
        recorder->get(t);

        sharedData->envOn = (envOnSink->inputs[0] == 1.0);

        return (sink->inputs[0] * 1.0);
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
