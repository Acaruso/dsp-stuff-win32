#pragma once

#include "src/audio/sample_buffer.hpp"
#include "src/audio/ugens.hpp"
#include "src/audio/wasapi_client.hpp"
#include "src/shared/shared_data.hpp"

class AudioService {
public:
    AudioService(
        WasapiClient& wasapiClient,
        SharedData* sharedData
    );
    void run();
    void fillSampleBuffer(size_t numSamplesToWrite);
    double getSample();
    double getTime();

private:
    WasapiClient& wasapiClient;
    SampleBuffer sampleBuffer;
    unsigned bufferSizeBytes{0};
    unsigned bufferSizeFrames{0};

    unsigned long sampleCounter{0};
    double secondsPerSample{0.0};
    SharedData* sharedData;

    // ugens
    Env env;
    AHREnv ampEnv;
    AHREnv modEnv;
    double freq{120.0};
    double r{0.0};
    bool trig{false};
};
