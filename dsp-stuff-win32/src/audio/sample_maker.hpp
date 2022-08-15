#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include "src/audio/ugens.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class SampleMaker {
public:
    SharedData* sharedData;
    unsigned samplesPerSecond = 0;
    double secondsPerSample = 0.0;

    AHREnv ampEnv;
    AHREnv modEnv;
    bool trig = false;
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

        unsigned ampSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);

        sharedData->sampleBuffer.resize(ampSamps, 0.0);
    }

    double makeSample(unsigned long sampleCounter, std::string& message) {
        // TODO: don't use string for message, use enum or something
        if (message == "trig") {
            trig = true;
            r = getRand();
            sharedBufferIdx = 0;
        } else {
            trig = false;
        }

        double t = getTime(sampleCounter);

        double w = twoPi * freq;

        double theta = sin(w * t * 0.5) * modEnv.get(trig, 1, 50, 200, t) * 8 * r;

        double sinSig = sin((w * t) + theta);

        double envSig = ampEnv.get(trig, ampA, ampH, ampR, t);

        double sig = sinSig * envSig;

        if (ampEnv.on) {
            if (sharedBufferIdx < sharedData->sampleBuffer.size()) {
                sharedData->sampleBuffer[sharedBufferIdx] = sig;
                sharedBufferIdx++;
            }
        }

        sharedData->envOn = ampEnv.on;

        double attenuatedSig = sig * 0.5;

        return attenuatedSig;
    }

    double getTime(unsigned long sampleCounter) {
        return (double)(sampleCounter) * secondsPerSample;
    }
};
