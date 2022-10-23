#pragma once

#include <vector>

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
#include "src/audio/ugens/ugen_utils.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc_freq_mod.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/white_noise_osc.hpp"

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeSinOscEnv(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    float freq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 3);

    BaseUgen* pUgen = new WavetableOsc(ugenCtx, &ugenCtx->wavetables.sin, freq);
    pUgen->setLevel(level);
    int osc = m->addUgen(pUgen);

    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData);
    int ampEnv = m->addUgen(new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration));

    int ampEnvOut0 = m->addUgen(new Split(ugenCtx, 2));
    int vca = m->addUgen(new Mult(ugenCtx));
    int managerIn0 = m->addUgen(new Split(ugenCtx, 2));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    ampEnv,     0,
            managerIn0, 1,    osc,        0,
            MANAGER,    1,    osc,        1,
            ampEnv,     0,    ampEnvOut0, 0,
            ampEnvOut0, 0,    vca,        0,
            osc,        0,    vca,        1,
            vca,        0,    MANAGER,    0,
            ampEnvOut0, 1,    MANAGER,    1,
            ampEnv,     1,    MANAGER,    2
        }
    );

    return m;
}

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio

inline UgenManager* makeSinOscEnvFreqEnv(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 1);

    int managerIn0 = m->addUgen(new Split(ugenCtx, 3));

    // create osc
    BaseUgen* pOsc = new WavetableOscFreqMod(ugenCtx, &ugenCtx->wavetables.sin);
    pOsc->setLevel(level);
    int osc = m->addUgen(pOsc);

    // create ampEnv
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData);
    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ugenCtx, ampEnvWt, ampEnvData.duration)
    );

    // create freqEnv
    std::vector<float>* freqEnvWt = new std::vector<float>;
    makeAHRWavetable(*freqEnvWt, 1024, freqEnvData);
    int freqEnv = m->addUgen(
        "freqEnv",
        new WavetableEnv(ugenCtx, freqEnvWt, freqEnvData.duration)
    );

    int scale = m->addUgen(new Scale(ugenCtx, 0, 1, lowFreq, highFreq));

    int vca = m->addUgen(new Mult(ugenCtx));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    ampEnv,     0,
            managerIn0, 1,    freqEnv,    0,
            managerIn0, 2,    osc,        0,
            MANAGER,    1,    osc,        1,
            freqEnv,    0,    scale,      0,
            scale,      0,    osc,        2,
            ampEnv,     0,    vca,        0,
            osc,        0,    vca,        1,
            vca,        0,    MANAGER,    0
        }
    );

    return m;
}

inline UgenManager* makeWhiteNoiseOscEnv(
    UgenCtx* ugenCtx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 1);

    BaseUgen* pWhiteNoiseOsc = new WhiteNoiseOsc(ugenCtx);
    pWhiteNoiseOsc->setLevel(level);
    int whiteNoiseOsc = m->addUgen(pWhiteNoiseOsc);

    // create ampEnv
    std::vector<float>* ampEnvWt = new std::vector<float>;
    makeAHRWavetable(*ampEnvWt, 1024, ampEnvData);
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
