#pragma once

#include "../lib/readerwriterqueue.h"

class SharedData {
public:
    moodycamel::ReaderWriterQueue<std::string> toAudio;
    size_t sampleBufferSize = 0;
    double* sampleBuffer = nullptr;

    void initSampleBuffer(size_t size) {
        sampleBufferSize = size;
        sampleBuffer = new double[size];
    }

    void destroySampleBuffer() {
        delete[] sampleBuffer;
    }
};
