#pragma once

#include "src/audio/audio_util.hpp"

// a, h, r, and duration are in ms
struct AHRData {
    float a = 0.0f;
    float h = 0.0f;
    float r = 0.0f;

    float getDurationMs() {
        return a + h + r;
    }

    unsigned getDurationSamps() {
        return mstosamps(a + h + r);
    }
};

struct AHRScaleData {
    float a = 0.0f;
    float h = 0.0f;
    float r = 0.0f;
    float low = 0.0f;
    float high = 0.0f;

    float getDurationMs() {
        return a + h + r;
    }

    unsigned getDurationSamps() {
        return mstosamps(a + h + r);
    }
};
