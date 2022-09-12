#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "src/audio/ugens/ugen_manager.hpp"
#include "src/lib/readerwriterqueue.h"

struct SharedBuffer {
    std::vector<double> data;
    bool active = false;
};

enum ToAudioMessageType {
    AM_NO_MESSAGE,
    AM_TRIG,
    AM_QUIT
};

struct ToAudioMessage {
    ToAudioMessageType type = AM_NO_MESSAGE;
    uint64_t param1;
    uint64_t param2;
};

enum ToMainMessageType {
    TM_NO_MESSAGE,
    TM_INIT_FINISHED
};

struct ToMainMessage {
    ToMainMessageType type = TM_NO_MESSAGE;
    uint64_t param1;
    uint64_t param2;
};

struct SharedData {
    moodycamel::ReaderWriterQueue<ToAudioMessage> toAudio;
    moodycamel::ReaderWriterQueue<ToMainMessage> toMain;
    UgenManager rootUgen;
};
