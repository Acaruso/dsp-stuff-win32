#pragma once

#include "src/audio/audio_util.hpp"

// a, h, r, and duration are in ms
struct AHRData {
    float a;
    float h;
    float r;

    float getDurationMs() {
        return a + h + r;
    }

    unsigned getDurationSamps() {
        return mstosamps(a + h + r);
    }
};
