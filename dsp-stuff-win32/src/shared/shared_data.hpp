#pragma once

#include <unordered_map>
#include <vector>

#include "src/lib/readerwriterqueue.h"

struct SharedBuffer {
    std::vector<double> data;
    bool active = false;
};

struct SharedData {
    moodycamel::ReaderWriterQueue<std::string> toAudio;
    std::unordered_map<int, SharedBuffer> sharedBuffers;
};
