#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/scale.hpp"
#include "src/audio/ugens/split.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc_freq_mod.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/white_noise_osc.hpp"
#include "src/audio/ugens/wt_sin.hpp"

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio

inline UgenManager* makeOscEnv2(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq
) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 1);

    // create osc
    int osc = m->addUgen(new WavetableOscFreqMod(ugenCtx, &ugenCtx->wavetables.sin));

    // create ampEnv
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData.a, ampEnvData.h, ampEnvData.r);
    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration)
    );

    // create freqEnv
    std::vector<float>* freqEnvWt = new std::vector<float>;
    makeAHRWavetable(*freqEnvWt, 1024, freqEnvData.a, freqEnvData.h, freqEnvData.r);
    int freqEnv = m->addUgen(
        "freqEnv",
        new WavetableEnv(ugenCtx, freqEnvWt, freqEnvData.duration)
    );

    // create scale
    int scale = m->addUgen(new Scale(ugenCtx, 0, 1, lowFreq, highFreq));

    // connect freqEnv to scale
    m->connect(freqEnv, 0, scale, 0);

    // connect scale to osc freq modulation
    m->connect(scale, 0, osc, 2);

    // create vca
    int vca = m->addUgen(new Mult(ugenCtx));

    // connect osc and env to vca
    m->connect(osc, 0, vca, 0);
    m->connect(ampEnv, 0, vca, 1);

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 3));
    m->connectIn(0, in0split, 0);
    m->connect(in0split, 0, ampEnv, 0);
    m->connect(in0split, 1, freqEnv, 0);
    m->connect(in0split, 2, osc, 0);

    // in[1] - fm mod
    m->connectIn(1, osc, 1);

    // out[0] - audio
    m->connectOut(vca, 0, 0);

    return m;
}

// in[0]  - trig
// out[0] - audio

// inline UgenManager* makeWhiteNoiseOscEnv(
//     UgenCtx* ugenCtx,
//     AHRData ampEnvData
// ) {
//     UgenManager* m = new UgenManager(ugenCtx, 1, 1);

//     std::vector<float>* whiteNoiseWt = new std::vector<float>;

//     // makeWhiteNoiseWavetable(*whiteNoiseWt, 2048 * 4);
//     makeWhiteNoiseWavetable(*whiteNoiseWt, samplesPerSecond);

//     // create white noise osc
//     int whiteNoiseOsc = m->addUgen(new WavetableOsc(ugenCtx, whiteNoiseWt, 1));

//     // create ampEnv
//     std::vector<float>* ampEnvWt = new std::vector<float>;
//     makeAHRWavetable(*ampEnvWt, 1024, ampEnvData.a, ampEnvData.h, ampEnvData.r);
//     int ampEnv = m->addUgen(
//         "ampEnv",
//         new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration)
//     );

//     // create vca
//     int vca = m->addUgen(new Mult(ugenCtx));

//     // connect osc and env outs to vca ins
//     m->connect(whiteNoiseOsc, 0, vca, 0);
//     m->connect(ampEnv, 0, vca, 1);

//     // in[0] - trig
//     m->connectIn(0, ampEnv, 0);

//     // out[0] - audio
//     m->connectOut(vca, 0, 0);

//     return m;
// }

inline UgenManager* makeWhiteNoiseOscEnv(
    UgenCtx* ugenCtx,
    AHRData ampEnvData
) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 1);

    int whiteNoiseOsc = m->addUgen(new WhiteNoiseOsc(ugenCtx));

    // create ampEnv
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData.a, ampEnvData.h, ampEnvData.r);
    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration)
    );

    // create vca
    int vca = m->addUgen(new Mult(ugenCtx));

    // connect osc and env outs to vca ins
    m->connect(whiteNoiseOsc, 0, vca, 0);
    m->connect(ampEnv, 0, vca, 1);

    // in[0] - trig
    m->connectIn(0, ampEnv, 0);

    // out[0] - audio
    m->connectOut(vca, 0, 0);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvWaveshaper(UgenCtx* ugenCtx, AHRData ampEnvData, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    // create stuff /////////////////////////////////////////////////

    // create osc
    int osc = m->addUgen(new WavetableOsc(ugenCtx, &ugenCtx->wavetables.sin, freq));

    // create ampEnv and split out0
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData.a, ampEnvData.h, ampEnvData.r);
    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration)
    );

    int ampEnv0Split = m->addUgen(new Split(ugenCtx, 2));

    m->connect(ampEnv, 0, ampEnv0Split, 0);

    // create vca
    int vca = m->addUgen(new Mult(ugenCtx));

    // connect stuff ////////////////////////////////////////////////

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 2));
    m->connectIn(0, in0split, 0);

    m->connect(in0split, 0, osc, 0);
    m->connect(in0split, 1, ampEnv, 0);

    // connect env and osc to vca
    m->connect(ampEnv0Split, 0, vca, 0);
    m->connect(osc, 0, vca, 1);

    // create waveshaper
    int waveshaper = m->addUgen(new Waveshaper(ugenCtx, &ugenCtx->wavetables.tanh));

    m->connect(vca, 0, waveshaper, 0);

    // out[0] - audio
    m->connectOut(waveshaper, 0, 0);

    // out[1] - amp env signal
    m->connectOut(ampEnv0Split, 1, 1);

    // out[2] - amp env on/off
    m->connectOut(ampEnv, 1, 2);

    return m;
}

inline UgenManager* addRecorders(UgenCtx* ugenCtx, UgenManager* pOsc, unsigned size) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 1);

    int osc = m->addUgen(pOsc);
    m->connectIn(0, osc, 0);

    int osc0split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 0, osc0split, 0);

    int osc2split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 2, osc2split, 0);

    int recorder1 = m->addUgen("recorder1", new Recorder(ugenCtx, size));
    int recorder2 = m->addUgen("recorder2", new Recorder(ugenCtx, size));

    m->connect(osc0split, 0, recorder1, 0);
    m->connect(osc2split, 0, recorder1, 1);

    m->connect(osc, 1, recorder2, 0);
    m->connect(osc2split, 1, recorder2, 1);

    m->connectOut(osc0split, 1, 0);

    return m;
}
