// #pragma once

// #include <vector>

// #include "src/audio/audio_util.hpp"

// inline void makeAHRWavetable(std::vector<float>wavetable, float a, float h, float r) {
//     int attackSamps = mstosamps(a);
//     int holdSamps = mstosamps(h);
//     int releaseSamps = mstosamps(r);

//     int attackTimeWt = wavetable.size() * ((float)attackSamps / (float)attackHoldReleaseSamps);
//     int holdTimeWt = wavetable.size() * ((float)holdSamps / (float)attackHoldReleaseSamps);
//     int releaseTimeWt = wavetable.size() * ((float)releaseSamps / (float)attackHoldReleaseSamps);

//     float attackDeltaWt = 1.0f / (float)attackTimeWt;
//     float releaseDeltaWt = 1.0f / (float)releaseTimeWt;

//     float linearSig = 0.0f;
//     float sig = 0.0f;

//     for (int i = 0; i < wtSizeToFill; i++) {
//         wavetable[i] = sig;

//         if (i < attackTimeWt) {
//             linearSig += attackDeltaWt;
//             sig = sqrt(linearSig);
//         } else if (i < attackTimeWt + holdTimeWt) {
//             sig = 1.0f;
//         } else if (i < attackTimeWt + holdTimeWt + releaseTimeWt) {
//             linearSig -= releaseDeltaWt;
//             sig = linearSig * linearSig;
//         }
//     }
// }
