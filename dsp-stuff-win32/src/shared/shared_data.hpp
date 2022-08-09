#pragma once

#include "../lib/readerwriterqueue.h"

class SharedData {
public:
    moodycamel::ReaderWriterQueue<std::string> toAudio;
    static constexpr size_t sampleBufferSize{512};
    double sampleBuffer[sampleBufferSize]{0};
};
