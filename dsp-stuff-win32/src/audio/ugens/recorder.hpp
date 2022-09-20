#pragma once

#include <iostream>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/shared_data.hpp"

// in[0] - input signal
// in[1] - on/off

class Recorder : public BaseUgen {
public:
    SharedAudioBuffer buffer;
    int idx = 0;

    Recorder() {
        resizeIns(2);
    }

    void run(unsigned sampleCounter) {
        buffer.active = in[1][0] == 1.0f;

        if (buffer.active) {
            for (int i = 0; i < bufferSize; ++i) {
                if (idx < buffer.data.size()) {
                    buffer.data[idx] = in[0][i];
                    idx++;
                }
            }
        } else {
            idx = 0;
        }
    }
};
