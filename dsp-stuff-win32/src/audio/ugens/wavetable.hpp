// #pragma once

// #include <cmath>
// #include <vector>

// #include "src/audio/audio_util.hpp"
// #include "src/audio/ugens/base_ugen.hpp"
// #include "src/shared/shared_constants.hpp"

// class Wavetable : public BaseUgen {
// public:
//     std::vector<float>* wavetable;
//     bool on = false;
//     float sig;
//     unsigned timer = 0;

//     Wavetable(UgenCtx* _ugenCtx) {
//         ugenCtx = _ugenCtx;
//         numIns = 2;
//         numOuts = 1;
//         allocateBuffers("Wavetable");
//     }

//     void run(unsigned sampleCounter) override {
//         auto& d = ugenCtx->bufferAllocator.data;
//         unsigned in0 = in[0];
//         unsigned out0 = out[0];
//         unsigned out1 = out[1];

//         if (READ_IN(d, in0, 0) == 1.0f) {
//             trigger();
//         }

//         if (!on) {
//             fillBuffer(d, out0, bufferSize, 0.0f);
//         } else {
//             for (int i = 0; i < bufferSize; ++i) {
//                 if (timer < wavetable->size()) {
//                     f_wtIdx = timer * ratio;
//                     wtIdx = (int)f_wtIdx;
//                     sig = wavetable[wtIdx] + ((f_wtIdx - wtIdx) * wavetable[wtIdx + 1]);
//                 } else {
//                     sig = 0.0f;
//                     on = false;
//                 }

//                 ++timer;

//                 WRITE_OUT(d, out0, i, sig);
//             }
//         }

//         WRITE_OUT(d, out1, 0, on ? 1.0f : 0.0f);
//     }

//     inline void trigger() {
//         on = true;
//         sig = 0.0f;
//         timer = 0;
//     }
// };
