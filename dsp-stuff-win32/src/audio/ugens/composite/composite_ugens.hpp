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

// in[0]  - trig
// in[1]  - fm mod
// in[2]  - freq
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeSinOscFreqInEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 3, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 2));

    int osc = m->addUgen(new WavetableOscFreqMod(ctx, ctx->wavetables.sin, level));

    int ampEnv = m->addUgen(new AHRExpEnv(ctx, ampEnvData));

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int vca = m->addUgen(new Mult(ctx));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    ampEnv,     0,    // amp trigger
            managerIn0, 1,    osc,        0,    // phase reset
            MANAGER,    1,    osc,        1,    // phase mod
            MANAGER,    2,    osc,        2,    // freq mod
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
// in[2]  - freq
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeWavetableOscFreqInEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    std::vector<float>* wavetable,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 3, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 2));

    int osc = m->addUgen(new WavetableOscFreqMod(ctx, wavetable, level));

    int ampEnv = m->addUgen(new AHRExpEnv(ctx, ampEnvData));

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int vca = m->addUgen(new Mult(ctx));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    ampEnv,     0,    // amp trigger
            managerIn0, 1,    osc,        0,    // phase reset
            MANAGER,    1,    osc,        1,    // phase mod
            MANAGER,    2,    osc,        2,    // freq mod
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
// in[1]  - carrier freq
// in[2]  - mod freq
// out[0] - audio

inline UgenManager* makeTwoOp(
    UgenCtx* ctx,
    AHRData ampEnvData,
    AHRData modEnvData,
    float level=1.0f
) {
    float fmAmount = 4.0f;

    UgenManager* m = new UgenManager(ctx, 3, 3);

    int trig = m->addUgen(new Split(ctx, 4));

    int car = m->addUgen(new WavetableOscFreqMod(ctx, ctx->wavetables.sin));

    int mod = m->addUgen(new WavetableOscFreqMod(ctx, ctx->wavetables.sin));

    int ampEnv = m->addUgen(new AHRExpEnv(ctx, ampEnvData));

    int modEnv = m->addUgen(new AHRExpEnv(ctx, modEnvData));

    int ampVca = m->addUgen(new Mult(ctx, level));

    int modVca = m->addUgen("fmAmount", new Mult(ctx, fmAmount));

    m->connect(
        std::vector<int>{
            MANAGER, 0,    trig,    0,
            trig,    0,    car,     0,
            trig,    1,    mod,     0, 
            trig,    2,    ampEnv,  0,
            trig,    3,    modEnv,  0,
            MANAGER, 1,    car,     2,
            MANAGER, 2,    mod,     2,
            mod,     0,    modVca,  0,
            modEnv,  0,    modVca,  1,
            modVca,  0,    car,     1,
            ampEnv,  0,    ampVca,  0,
            car,     0,    ampVca,  1,
            ampVca,  0,    MANAGER, 0
        }
    );

    return m;
}

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeSinOscEnvFreqEnvWTEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 2, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 3));

    int osc = m->addUgen(new WavetableOscFreqMod(ctx, ctx->wavetables.sin, level));

    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.getDurationMs())
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int freqEnv = m->addUgen(
        "freqEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, freqEnvData), freqEnvData.getDurationMs())
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
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeSinOscEnvFreqEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 2, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 3));

    int osc = m->addUgen(new WavetableOscFreqMod(ctx, ctx->wavetables.sin, level));

    int ampEnv = m->addUgen(
        "ampEnv",
        new AHRExpEnv(ctx, ampEnvData)
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int freqEnv = m->addUgen(
        "freqEnv",
        new AHRExpEnv(ctx, freqEnvData)
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
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeWavetableOscEnvFreqEnv(
    UgenCtx* ctx,
    std::vector<float>* wavetable,
    AHRData ampEnvData,
    AHRData freqEnvData,
    float lowFreq,
    float highFreq,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 2, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 3));

    int osc = m->addUgen(new WavetableOscFreqMod(ctx, wavetable, level));

    int ampEnv = m->addUgen(
        "ampEnv",
        new AHRExpEnv(ctx, ampEnvData)
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int freqEnv = m->addUgen(
        "freqEnv",
        new AHRExpEnv(ctx, freqEnvData)
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
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeWhiteNoiseOscEnvWTEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int osc = m->addUgen(new WhiteNoiseOsc(ctx, level));

    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.getDurationMs())
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int vca = m->addUgen(new Mult(ctx));

    m->connect(
        std::vector<int> {
            MANAGER,    0,    ampEnv,     0,
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
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeWhiteNoiseOscEnv(
    UgenCtx* ctx,
    AHRData ampEnvData,
    float level=1.0f
) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int osc = m->addUgen(new WhiteNoiseOsc(ctx, level));

    int ampEnv = m->addUgen(
        "ampEnv",
        new WavetableEnv(ctx, makeAHRWavetable(1024, ampEnvData), ampEnvData.getDurationMs())
    );

    int ampEnvOut0 = m->addUgen(new Split(ctx, 2));

    int vca = m->addUgen(new Mult(ctx));

    m->connect(
        std::vector<int> {
            MANAGER,    0,    ampEnv,     0,
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
