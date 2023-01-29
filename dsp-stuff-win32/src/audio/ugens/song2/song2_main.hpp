#pragma once

#include <cmath>
#include <iostream>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// out[0] - audio signal

namespace Song2 {

class Main : public BaseUgen {
public:
    Notes notes;
    Saw saw;
    Square square;
    Triangle triangle{1.0f};
    Env env{AHRData{0.0f, 20.0f, 2000.0f}};

    float sig = 0.0f;
    float triSig = 0.0f;

    Main(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float _level=1.0f
    ) {
        typeStr = "Song2::Main";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if ((sampleCounter + i) % 50000 == 0) {
                saw.setFreq(notes.getFreq());
                square.setFreq(notes.getFreq());
                // triangle.setFreq(notes.getFreq() * 0.005);
                notes.incNote();
                env.trigger();
            }

            if (!env.on) {
                WRITE_OUT(d, out0, i, 0.0f);
            } else {
                // sig = saw.get() * env.get() * level;
                triSig = triangle.get() * env.get() * level;
                // square.setFlip(
                //     ((triSig * 0.5f) + 1.0f) * 0.5f
                // );
                sig = square.get() * env.get() * level;
                WRITE_OUT(d, out0, i, sig);
            }

            saw.run();
            square.run();
            triangle.run();
            env.run();
        }
    }
};

}
