#include "audio_service.hpp"

#include <cstdlib>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_util.hpp"

AudioService::AudioService(
    WasapiClient& wasapiClient,
    SharedData* sharedData
)
    : wasapiClient(wasapiClient), sharedData(sharedData)
{
    unsigned long samplesPerSecond = wasapiClient.waveFormat.Format.nSamplesPerSec;
    secondsPerSample = 1.0 / (double)samplesPerSecond;

    bufferSizeBytes = wasapiClient.getBufferSizeBytes();
    sampleBuffer.init(bufferSizeBytes);

    bufferSizeFrames = wasapiClient.getBufferSizeFrames();

    ampSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);

    // sharedData->initSampleBuffer(512);
    sharedData->initSampleBuffer(ampSamps);

    bufferWriteRate = ampSamps / sharedData->sampleBufferSize;
    std::cout << "bufferWriteRate: " << bufferWriteRate << std::endl;
}

void AudioService::run() {
    sampleBuffer.zero();

    wasapiClient.writeBuffer(sampleBuffer.buffer, bufferSizeFrames);

    wasapiClient.startPlaying();

    std::string s;

    // main loop:
    while (true) {
        WaitForSingleObject(wasapiClient.hEvent, INFINITE);

        // handle events from main thread
        trig = false;
        if (sharedData->toAudio.try_dequeue(s)) {
            if (s == "quit") {
                std::cout << "audio thread: " << s << std::endl;
                break;
            }

            if (s == "trig") {
                trig = true;
            }
        }

        unsigned numPaddingFrames = wasapiClient.getCurrentPadding();

        // recall that each elt of buffer stores 1 sample
        // frame is 2 samples -> 1 for each channel
        // so numSamplesToWrite is 2x numFramesToWrite

        unsigned numFramesToWrite = bufferSizeFrames - numPaddingFrames;

        unsigned numSamplesToWrite = numFramesToWrite * 2;

        fillSampleBuffer(numSamplesToWrite);

        wasapiClient.writeBuffer(sampleBuffer.buffer, numFramesToWrite);
    }

    wasapiClient.stopPlaying();
}

void AudioService::fillSampleBuffer(size_t numSamplesToWrite) {
    unsigned numChannels = 2;

    for (int i = 0; i < numSamplesToWrite; i += numChannels) {

        double sig = getSample();

        unsigned samp = scaleSignal(sig);

        sampleBuffer.buffer[i] = samp;       // L
        sampleBuffer.buffer[i + 1] = samp;   // R

        sampleCounter++;
    }
}

// double AudioService::getSample() {
//     if (trig) {
//         r = getRand();
//     }

//     double t = getTime();

//     double w = twoPi * freq;

//     double theta = sin(w * t * 0.5) * modEnv.get(trig, 1, 50, 200, t) * 8 * r;

//     double sinSig = sin((w * t) + theta);

//     double envSig = ampEnv.get(trig, ampA, ampH, ampR, t);

//     double sig = sinSig * envSig * 0.5;

//     return sig;
// }

double AudioService::getSample() {
    if (trig) {
        r = getRand();
        bufferWriteIdx = 0;
    }

    double t = getTime();

    double w = twoPi * freq;

    double theta = sin(w * t * 0.5) * modEnv.get(trig, 1, 50, 200, t) * 8 * r;

    double sinSig = sin((w * t) + theta);

    double envSig = ampEnv.get(trig, ampA, ampH, ampR, t);

    double sig = sinSig * envSig;

    if (ampEnv.on) {
        if (bufferWriteCounter == 0) {
            if (bufferWriteIdx < sharedData->sampleBufferSize) {
                sharedData->sampleBuffer[bufferWriteIdx] = sig;
                bufferWriteIdx++;
            }
        }
        bufferWriteCounter = (bufferWriteCounter + 1) % bufferWriteRate;
    }

    double attenuatedSig = sig * 0.5;

    return attenuatedSig;
}

double AudioService::getTime() {
    return double(sampleCounter) * secondsPerSample;
}
