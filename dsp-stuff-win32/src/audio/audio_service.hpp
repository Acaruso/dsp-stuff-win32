#pragma once

#include "src/audio/sample_buffer.hpp"
#include "src/audio/sample_maker.hpp"
#include "src/audio/wasapi_client.hpp"
#include "src/shared/shared_data.hpp"

class AudioService {
public:
    AudioService(WasapiClient& wasapiClient, SharedData* sharedData);
    void run();

private:
    WasapiClient& wasapiClient;
    SharedData* sharedData;
    SampleBuffer sampleBuffer;
    SampleMaker sampleMaker;

    unsigned bufferSizeBytes{0};
    unsigned bufferSizeFrames{0};

    unsigned long sampleCounter{0};

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    double avgTime = 0.0;
    unsigned avgCount = 0;

    void fillSampleBuffer(size_t numSamplesToWrite);
    bool handleMessage(ToAudioMessage& message);

    void beginTimer();
    void endTimer();
};
