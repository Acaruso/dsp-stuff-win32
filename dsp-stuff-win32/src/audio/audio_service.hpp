#pragma once

#include "src/audio/sample_buffer.hpp"
#include "src/audio/sample_maker.hpp"
#include "src/audio/wasapi_client.hpp"
#include "src/shared/shared_data.hpp"

class AudioService {
public:
    AudioService(
        WasapiClient& wasapiClient,
        SharedData* sharedData
    );
    void run();
    void fillSampleBuffer(size_t numSamplesToWrite, ToAudioMessage& message);

private:
    WasapiClient& wasapiClient;
    SampleBuffer sampleBuffer;
    SampleMaker sampleMaker;

    unsigned bufferSizeBytes{0};
    unsigned bufferSizeFrames{0};

    unsigned long sampleCounter{0};
    unsigned long samplesPerSecond{0};
    double secondsPerSample{0.0};
    SharedData* sharedData;
};
