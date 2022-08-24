#include "audio_service.hpp"

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

        // TODO: what if there's more than one event in the queue?
        //       additional events will not be processed until next loop iteration
        if (sharedData->toAudio.try_dequeue(message)) {
            quit = handleMessage(message);
            if (quit) {
                std::cout << "audio thread quitting" << std::endl;
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

        message.type = NO_MESSAGE;
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
        case NO_MESSAGE:
            break;
    }
    return false;
}

void AudioService::fillSampleBuffer(size_t numSamplesToWrite) {
    unsigned numChannels = 2;

    for (int i = 0; i < numSamplesToWrite; i += numChannels) {
        double sig = sampleMaker.makeSample(sampleCounter);

        unsigned samp = scaleSignal(sig);

        sampleBuffer.buffer[i] = samp;       // L
        sampleBuffer.buffer[i + 1] = samp;   // R

        sampleCounter++;
    }
}
