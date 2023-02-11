#pragma once

#include <vector>

#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/main/util.hpp"

namespace Song2 {
class Waveshaper : public BaseGen {
public:
    std::vector<float>* wavetable;
    int wtSize = 0;
    int idx = 0;
    float mult = 20;
    float level = 1;

    Waveshaper(std::vector<float>* _wavetable) {
        wavetable = _wavetable;
        wtSize = wavetable->size();
    }

    float get(float inSig) {
        inSig = inSig * mult;

        // TODO: lerp this?
        idx = (int)(((inSig + 1) * 0.5) * wtSize);

        idx = clamp(idx, 0, wtSize);

        return (*wavetable)[idx] * level;
    }

    void run() override {}
};

}
