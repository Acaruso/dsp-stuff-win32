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

    Recorder(UgenCtx* _ugenCtx) {
        ugenCtx = _ugenCtx;
        numIns = 2;
        allocateBuffers("Recorder");
    }

    void run(unsigned sampleCounter) {
        auto& d = ugenCtx->bufferAllocator.data;

        unsigned in0 = in[0];
        unsigned in1 = in[1];

        buffer.active = READ_IN(d, in1, 0) == 1.0f;

        if (buffer.active) {
            for (int i = 0; i < bufferSize; ++i) {
                if (idx < buffer.data.size()) {
                    buffer.data[idx] = READ_IN(d, in0, i);
                    idx++;
                }
            }
        } else {
            idx = 0;
        }
    }

    // void run(unsigned sampleCounter) {
    //     auto& d = ugenCtx->bufferAllocator.data;

    //     unsigned in0 = in[0];
    //     unsigned in1 = in[1];

    //     // buffer.active = READ_IN(d, in1, 0) == 1.0f;

    //     // if (buffer.active) {
    //     //     for (int i = 0; i < bufferSize; ++i) {
    //     //         if (idx < buffer.data.size()) {
    //     //             buffer.data[idx] = READ_IN(d, in0, i);
    //     //             idx++;
    //     //         }
    //     //     }
    //     // } else {
    //     //     idx = 0;
    //     // }

    //     // for (int i = 0; i < bufferSize; ++i) {
    //     //     buffer.active = READ_IN(d, in1, i) == 1.0f;

    //     //     if (buffer.active) {
    //     //         if (idx < buffer.data.size()) {
    //     //             buffer.data[idx] = READ_IN(d, in0, i);
    //     //             ++idx;
    //     //         }
    //     //     } else {
    //     //         idx = 0;
    //     //     }
    //     // }

    //     // for (int i = 0; i < bufferSize; ++i) {
    //     //     buffer.active = READ_IN(d, in1, i) == 1.0f;

    //     //     if (buffer.active && idx < buffer.data.size()) {
    //     //         buffer.data[idx] = READ_IN(d, in0, i);
    //     //         ++idx;
    //     //     } else {
    //     //         idx = 0;
    //     //     }
    //     // }
        
    //     for (int i = 0; i < bufferSize; ++i) {
    //         buffer.active = READ_IN(d, in1, 0) == 1.0f;

    //         if (buffer.active && idx < buffer.data.size()) {
    //             buffer.data[idx] = READ_IN(d, in0, i);
    //             ++idx;
    //         } else {
    //             idx = 0;
    //         }
    //     }
    // }
};
