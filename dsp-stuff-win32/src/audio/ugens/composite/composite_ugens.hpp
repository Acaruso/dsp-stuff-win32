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
    UgenCtx* ctx,
    AHRData ampEnvData,
    float freq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 2, 3);

    BaseUgen* pUgen = new WavetableOsc(ctx, &ctx->wavetables.sin, freq);
    pUgen->setLevel(level);
    int osc = m->addUgen(pUgen);

    int ampEnv = m->addUgen(
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.duration)
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));
    int vca = m->addUgen(new Mult(ctx));
    int managerIn0 = m->addUgen(new Split(ctx, 2));

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
    UgenCtx* ctx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 2, 1);

    int managerIn0 = m->addUgen(new Split(ctx, 3));

    // create osc
    BaseUgen* pOsc = new WavetableOscFreqMod(ctx, &ctx->wavetables.sin);
    pOsc->setLevel(level);
    int osc = m->addUgen(pOsc);

    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.duration)
    );

    int freqEnv = m->addUgen(
        "freqEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, freqEnvData), freqEnvData.duration)
    );

    int scale = m->addUgen(new Scale(ctx, 0, 1, lowFreq, highFreq));

    int vca = m->addUgen(new Mult(ctx));

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
    UgenCtx* ctx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 1);

    BaseUgen* pOsc = new WhiteNoiseOsc(ctx);
    pOsc->setLevel(level);
    int osc = m->addUgen(pOsc);

    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.duration)
    );

    int vca = m->addUgen(new Mult(ctx));

    m->connect(
        std::vector<int> {
            MANAGER, 0,    ampEnv,  0,
            ampEnv,  0,    vca,     0,
            osc,     0,    vca,     1,
            vca,     0,    MANAGER, 0
        }
    );

    return m;
}
