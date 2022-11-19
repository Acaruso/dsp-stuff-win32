#pragma once

#include <vector>

#include "src/audio/ugens/advanced/ahr_exp_env_scale.hpp"
#include "src/audio/ugens/advanced/ahr_exp_env_vca_scale.hpp"
#include "src/audio/ugens/advanced/ahr_exp_env_vca.hpp"
#include "src/audio/ugens/advanced/wavetable_op.hpp"
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
#include "src/audio/ugens/ugen_utils.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc_freq_mod.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/white_noise_osc.hpp"

inline AHRScaleData ahrDataToAhrScaleData(AHRData ahrData) {
    return {
        ahrData.a,
        ahrData.h,
        ahrData.r,
        0.0f,
        1.0f
    };
}

// in[0]  - trig
// out[0] - audio

inline UgenManager* makeWtOpFreqEnv(
    UgenCtx* ctx,
    std::vector<float>* wavetable,
    AHRData ampEnvData,
    AHRScaleData freqEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 1);

    int s_trig = m->addUgen(new Split(ctx, 2));

    int wtOp = m->addUgen(
        "ampEnv",
        new WavetableOp(
            ctx,
            wavetable,
            ahrDataToAhrScaleData(ampEnvData),
            level
        )
    );

    int freqEnv = m->addUgen("freqEnv", new AHRExpEnvScale(ctx, freqEnvData));

    m->connect(
        std::vector<int>{
            MANAGER, 0,    s_trig,  0,
            s_trig,  0,    wtOp,    0,
            s_trig,  1,    freqEnv, 0,
            freqEnv, 0,    wtOp,    2,
            wtOp,    0,    MANAGER, 0
        }
    );

    return m;
}

// in[0]  - trig
// in[1]  - carrier freq
// in[2]  - mod freq
// out[0] - audio

inline UgenManager* makeTwoWtOp(
    UgenCtx* ctx,
    std::vector<float>* wavetable,
    AHRData ampEnvData,
    AHRScaleData modEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 3, 1);

    int s_trig = m->addUgen(new Split(ctx, 2));

    int car = m->addUgen(
        "ampEnv",
        new WavetableOp(
            ctx,
            wavetable,
            ahrDataToAhrScaleData(ampEnvData),
            level
        )
    );

    int mod = m->addUgen(
        "modEnv",
        new WavetableOp(
            ctx,
            wavetable,
            modEnvData
        )
    );

    m->connect(
        std::vector<int>{
            MANAGER, 0,    s_trig,  0,
            s_trig,  0,    car,     0,
            s_trig,  1,    mod,     0,
            MANAGER, 1,    car,     2,
            MANAGER, 2,    mod,     2,
            mod,     0,    car,     1,
            car,     0,    MANAGER, 0
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio

inline UgenManager* makeTwoWtOpTwoFreqEnv(
    UgenCtx* ctx,
    std::vector<float>* wavetable,
    AHRScaleData carAmpEnvData,
    AHRScaleData modAmpEnvData,
    AHRScaleData carFreqEnvData,
    AHRScaleData modFreqEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 1);

    int s_trig = m->addUgen(new Split(ctx, 4));

    int car = m->addUgen(
        "carAmpEnv",
        new WavetableOp(
            ctx,
            wavetable,
            carAmpEnvData,
            level
        )
    );

    int mod = m->addUgen(
        "modAmpEnv",
        new WavetableOp(
            ctx,
            wavetable,
            modAmpEnvData
        )
    );

    int carFreqEnv = m->addUgen("carFreqEnv", new AHRExpEnvScale(ctx, carFreqEnvData));

    int modFreqEnv = m->addUgen("modFreqEnv", new AHRExpEnvScale(ctx, modFreqEnvData));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    s_trig,     0,
            s_trig,     0,    car,        0,
            s_trig,     1,    mod,        0,
            s_trig,     2,    carFreqEnv, 0,
            s_trig,     3,    modFreqEnv, 0,
            carFreqEnv, 0,    car,        2,
            modFreqEnv, 0,    mod,        2,
            mod,        0,    car,        1,
            car,        0,    MANAGER,    0
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio

inline UgenManager* makeWhiteNoiseOp(
    UgenCtx* ctx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 1);
    
    int osc = m->addUgen(new WhiteNoiseOsc(ctx, level));
    int ampEnv = m->addUgen("ampEnv", new AHRExpEnvVca(ctx, ampEnvData));

    m->connect(
        std::vector<int> {
            MANAGER, 0,    ampEnv,  0,
            osc,     0,    ampEnv,  1,
            ampEnv,  0,    MANAGER, 0
        }
    );

    return m;
}

// old ////////////////////////////////////////////////////////////////////////

// in[0]  - trig
// in[1]  - phase mod
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

    int managerIn0 = m->addUgen(new Split(ctx, 2));

    int osc = m->addUgen(new WavetableOsc(ctx, ctx->wavetables.sin, freq, level));

    int ampEnv = m->addUgen(new AHRExpEnv(ctx, ampEnvData));

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int vca = m->addUgen(new Mult(ctx));

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
