#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "src/lib/readerwriterqueue.h"

enum ToAudioMessageType {
    NO_MESSAGE,
    AM_TRIG,
    AM_QUIT
};

struct ToAudioMessage {
    ToAudioMessageType type = NO_MESSAGE;
    uint64_t param1;
    uint64_t param2;
};

struct SharedBuffer {
    std::vector<double> data;
    bool active = false;
};

struct SharedData {
    moodycamel::ReaderWriterQueue<ToAudioMessage> toAudio;
    std::unordered_map<int, SharedBuffer> sharedBuffers;
};
