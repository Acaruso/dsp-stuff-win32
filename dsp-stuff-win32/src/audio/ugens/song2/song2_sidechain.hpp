#pragma once

#include "src/audio/ugens/song2/song2_base_gen.hpp"

namespace Song2 {

// amnt should be between 0.0 and 1.0
// 1.0 is full sidechaining -- will turn `signal`'s volume all the way down to 0
inline float sidechain(float signal, float mod, float amnt=1.0f) {
    return signal * ((mod * -(amnt)) + 1.0f);
}

}
