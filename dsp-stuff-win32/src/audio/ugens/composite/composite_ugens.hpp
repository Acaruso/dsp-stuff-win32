#pragma once

#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/split.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/ugen_utils.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/wt_sin.hpp"

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnv(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    float freq
) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 3);

    // create osc
    int osc = m->addUgen(new WavetableOsc(ugenCtx, &ugenCtx->wavetables.sin, freq));

    // create amp env
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData.a, ampEnvData.h, ampEnvData.r);
    int ampEnv = m->addUgen(new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.a + ampEnvData.h + ampEnvData.r));

    int env0split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(ampEnv, 0, env0split, 0);

    int vca = m->addUgen(new Mult(ugenCtx));

    // connect osc and env to vca
    m->connect(osc, 0, vca, 0);
    m->connect(env0split, 0, vca, 1);

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 2));
    m->connectIn(0, in0split, 0);
    m->connect(in0split, 0, ampEnv, 0);
    m->connect(in0split, 1, osc, 0);

    // in[1] - fm mod
    m->connectIn(1, osc, 1);

    // out[0] - audio
    m->connectOut(vca, 0, 0);

    // out[1] - amp env signal
    m->connectOut(env0split, 1, 1);

    // out[2] - amp env on/off
    m->connectOut(ampEnv, 1, 2);

    return m;
}
