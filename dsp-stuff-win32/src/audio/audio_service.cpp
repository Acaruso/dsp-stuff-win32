#include "audio_service.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/shared/shared_constants.hpp"
#include "src/shared/shared_util.hpp"

AudioService::AudioService(
    WasapiClient& wasapiClient,
    SharedData* sharedData
)
    : wasapiClient(wasapiClient), sharedData(sharedData)
{
    samplesPerSecond = wasapiClient.waveFormat.Format.nSamplesPerSec;
    secondsPerSample = 1.0 / (double)samplesPerSecond;

    bufferSizeBytes = wasapiClient.getBufferSizeBytes();
    sampleBuffer.init(bufferSizeBytes);

    bufferSizeFrames = wasapiClient.getBufferSizeFrames();

    sampleMaker.init(sharedData, samplesPerSecond, secondsPerSample);
}

void AudioService::run() {
    sampleBuffer.zero();

    wasapiClient.writeBuffer(sampleBuffer.buffer, bufferSizeFrames);

    wasapiClient.startPlaying();

    ToAudioMessage message;
    bool quit = false;

    // main loop:
    while (true) {
        WaitForSingleObject(wasapiClient.hEvent, INFINITE);
        // beginTimer();

        // TODO: what if there's more than one event in the queue?
        //       additional events will not be processed until next loop iteration
        if (sharedData->toAudio.try_dequeue(message)) {
            quit = handleMessage(message);
            if (quit) {
                std::cout << "audio thread quitting" << std::endl;
                double avgTimeMs = avgTime / 1000000.0;
                std::cout << "average time ms: " << avgTimeMs << std::endl;
                break;
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

        message.type = AM_NO_MESSAGE;

        // endTimer();
    }

    wasapiClient.stopPlaying();
}

bool AudioService::handleMessage(ToAudioMessage& message) {
    switch (message.type) {
        case AM_TRIG:
            sampleMaker.trigs[0] = true;
            break;
        case AM_QUIT:
            return true;
        case AM_NO_MESSAGE:
            break;
    }
    return false;
}

void AudioService::fillSampleBuffer(size_t numSamplesToWrite) {
    unsigned numChannels = 2;
    unsigned samp = 0;

    std::vector<double>& outVec = sampleMaker.makeSamples(sampleCounter);

    int j = 0;

    for (int i = 0; i < numSamplesToWrite; i += numChannels) {
        samp = scaleSignal(outVec[j]);
        j++;

        sampleBuffer.buffer[i] = samp;       // L
        sampleBuffer.buffer[i + 1] = samp;   // R

        sampleCounter++;
    }
}

void AudioService::beginTimer() {
    begin = std::chrono::steady_clock::now();
}

void AudioService::endTimer() {
    end = std::chrono::steady_clock::now();
    long long count = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

    if (avgCount > 1) {
        avgTime = (((double)(avgCount - 1) / (double)avgCount) * avgTime) + ((double)count / (double)avgCount);
    } else {
        avgTime = (double)count;
    }

    avgCount++;
}
