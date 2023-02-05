#pragma once


#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "src/audio/audio_constants.hpp"

// const unsigned scale = (1 << 23) - 1;
const unsigned scale = (1 << 23);

inline unsigned scaleSignal(float sig) {
    // float f = ((sig * 0.5f) + 0.5f) * scale;
    float f = sig * scale;
    unsigned u = (unsigned)f << 8;
    return u;
}

inline unsigned mstosamps(float ms) {
    return (unsigned)(ms * samplesPerMs);
}

inline unsigned mstosampsFloor1(float ms) {
    unsigned res = (unsigned)(ms * samplesPerMs);
    return (res > 0) ? res : 1;
}

inline float sampstoms(unsigned samps) {
    return samps * (1.0f / samplesPerMs);
}

inline double getTime(unsigned long sampleCounter) {
    return (double)(sampleCounter) * secondsPerSample;
}

inline void fillBuffer(
    std::vector<float>& data,
    int beginOffset,
    int size,
    float value
) {
    std::fill(
        data.begin() + beginOffset,
        data.begin() + beginOffset + size,
        value
    );
}

inline void copyBuffer(
    std::vector<float>& data,
    int sourceBeginOffset,
    int size,
    int destOffset
) {
    std::copy(
        data.begin() + sourceBeginOffset, 
        data.begin() + sourceBeginOffset + size,
        data.begin() + destOffset
    );
}

// windows.h defines `min` and `max` macros which cause issues
#undef min

inline bool isDenormal(float f) {
    return (
        f != 0 
        && (std::fabsf(f) < std::numeric_limits<float>::min())
    );
}

inline void printIfDenormal(float f, std::string s) {
    if (isDenormal(f)) {
        std::cout << "denormal: " << s << std::endl;
    }
}

#define LERP(y1, y2, frac) y1 + ((frac) * ((y2) - (y1)))

#define LERP_WT(wt, i, f) LERP((wt)[i], (wt)[i + 1], (f) - (i))

// see: https://musicinformationretrieval.com/midi_conversion_table.html
static std::vector<float> noteToFreqVec({
    // padding

    0,      // 0
    0,      // 1
    0,      // 2
    0,      // 3
    0,      // 4
    0,      // 5
    0,      // 6
    0,      // 7
    0,      // 8
    0,      // 9
    0,      // 10
    0,      // 11

    // octave 0

    16.352, // 12
    17.324, // 13
    18.354, // 14
    19.445, // 15
    20.602, // 16
    21.827, // 17
    23.125, // 18
    24.5,   // 19
    25.957, // 20
    27.5,   // 21
    29.135, // 22
    30.868, // 23

    // octave 1

    32.703, // 24
    34.648, // 25
    36.708, // 26
    38.891, // 27
    41.203, // 28
    43.654, // 29
    46.249, // 30
    48.999, // 31
    51.913, // 32
    55,     // 33
    58.27,  // 34
    61.735, // 35

    // octave 2

    65.406, // 36
    69.296, // 37
    73.416, // 38
    77.782, // 39
    82.407, // 40 -- low E on guitar
    87.307, // 41
    92.499, // 42
    97.999, // 43
    103.83, // 44
    110,    // 45
    116.54, // 46
    123.47, // 47

    // octave 3

    130.81, // 48
    138.59, // 49
    146.83, // 50
    155.56, // 51
    164.81, // 52
    174.61, // 53
    185,    // 54
    196,    // 55
    207.65, // 56
    220,    // 57
    233.08, // 58
    246.94, // 59

    // ocatave 4

    261.63, // 60 -- middle C
    277.18, // 61
    293.66, // 62
    311.13, // 63
    329.63, // 64 - E
    349.23, // 65
    369.99, // 66
    392,    // 67
    415.3,  // 68
    440,    // 69
    466.16, // 70
    493.88, // 71

    // octave 5

    523.25, // 72
    554.37, // 73
    587.33, // 74
    622.25, // 75
    659.26, // 76
    698.46, // 77
    739.99, // 78
    783.99, // 79
    830.61, // 80
    880,    // 81
    932.33, // 82
    987.77, // 83

    // octave 6

    1046.5, // 84
    1108.7, // 85
    1174.7, // 86
    1244.5, // 87
    1318.5, // 88
    1396.9, // 89
    1480,   // 90
    1568,   // 91
    1661.2, // 92
    1760,   // 93
    1864.7, // 94
    1975.5, // 95

    // octave 7

    2093,   // 96
    2217.5, // 97
    2349.3, // 98
    2489,   // 99
    2637,   // 100
    2793.8, // 101
    2960,   // 102
    3136,   // 103
    3322.4, // 104
    3520,   // 105
    3729.3, // 106
    3951.1  // 107
});

inline float noteToFreq(int note) {
    return noteToFreqVec[note];
}

static std::unordered_map<float, int> freqToNoteMap {
    // trig
    {1.0,    0},

    // octave 0

    {16.352, 12},
    {17.324, 13},
    {18.354, 14},
    {19.445, 15},
    {20.602, 16},
    {21.827, 17},
    {23.125, 18},
    {24.5,   19},
    {25.957, 20},
    {27.5,   21},
    {29.135, 22},
    {30.868, 23},

    // octave 1

    {32.703, 24},
    {34.648, 25},
    {36.708, 26},
    {38.891, 27},
    {41.203, 28},
    {43.654, 29},
    {46.249, 30},
    {48.999, 31},
    {51.913, 32},
    {55,     33},
    {58.27,  34},
    {61.735, 35},

    // octave 2

    {65.406, 36},
    {69.296, 37},
    {73.416, 38},
    {77.782, 39},
    {82.407, 40},
    {87.307, 41},
    {92.499, 42},
    {97.999, 43},
    {103.83, 44},
    {110,    45},
    {116.54, 46},
    {123.47, 47},

    // octave 3

    {130.81, 48},
    {138.59, 49},
    {146.83, 50},
    {155.56, 51},
    {164.81, 52},
    {174.61, 53},
    {185,    54},
    {196,    55},
    {207.65, 56},
    {220,    57},
    {233.08, 58},
    {246.94, 59},

    // octave 4

    {261.63, 60},
    {277.18, 61},
    {293.66, 62},
    {311.13, 63},
    {329.63, 64},
    {349.23, 65},
    {369.99, 66},
    {392,    67},
    {415.3,  68},
    {440,    69},
    {466.16, 70},
    {493.88, 71},

    // octave 5

    {523.25, 72},
    {554.37, 73},
    {587.33, 74},
    {622.25, 75},
    {659.26, 76},
    {698.46, 77},
    {739.99, 78},
    {783.99, 79},
    {830.61, 80},
    {880,    81},
    {932.33, 82},
    {987.77, 83},

    // octave 6

    {1046.5, 84},
    {1108.7, 85},
    {1174.7, 86},
    {1244.5, 87},
    {1318.5, 88},
    {1396.9, 89},
    {1480,   90},
    {1568,   91},
    {1661.2, 92},
    {1760,   93},
    {1864.7, 94},
    {1975.5, 95},

    // octave 7

    {2093,   96},
    {2217.5, 97},
    {2349.3, 98},
    {2489,   99},
    {2637,   100},
    {2793.8, 101},
    {2960,   102},
    {3136,   103},
    {3322.4, 104},
    {3520,   105},
    {3729.3, 106},
    {3951.1, 107},
};

inline int freqToNote(float freq) {
    if (freqToNoteMap.find(freq) == freqToNoteMap.end()) {
        return -1;
    } else {
        return freqToNoteMap[freq];
    }
}

inline float toSquare(float f) {
    if (f <= 0.0f) {
        return 0.0f;
    } else {
        return 1.0f;
    }
}
