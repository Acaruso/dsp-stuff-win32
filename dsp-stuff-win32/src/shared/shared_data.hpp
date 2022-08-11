#pragma once

#include <vector>

#include "../lib/readerwriterqueue.h"

class SharedData {
public:
    moodycamel::ReaderWriterQueue<std::string> toAudio;
    std::vector<double> sampleBuffer;
};
