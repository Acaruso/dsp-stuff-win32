#pragma once

#include <vector>

#include "src/lib/readerwriterqueue.h"

struct SharedBuffer {
    std::vector<double> data;
    bool active = false;
};

struct SharedData {
    moodycamel::ReaderWriterQueue<std::string> toAudio;
    std::vector<double> sampleBuffer;
    bool envOn = false;
};
