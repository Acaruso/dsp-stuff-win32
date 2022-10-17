#pragma once

#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/ahr_exp_env.hpp"
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
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc_freq_mod.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/wt_sin.hpp"

inline UgenManager* makeOscEnv2(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq
) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 3);

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
