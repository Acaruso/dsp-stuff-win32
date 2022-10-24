#pragma once

#include <vector>

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/const_mult.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/split.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/ugen_utils.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc.hpp"
#include "src/audio/ugens/wt_sin.hpp"

namespace CS {

static AHRData ahrData = { 100.0f, 200.0f, 50.0f };

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFM(UgenCtx* ctx, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 5));

    int carrier = m->addUgen(makeSinOscEnv(ctx, ahrData, freq));

    int mod1 = m->addUgen(makeSinOscEnv(ctx, ahrData, freq * 0.5f));
    int mod2 = m->addUgen(makeSinOscEnv(ctx, ahrData, freq * 2.0f));
    int mod3 = m->addUgen(makeSinOscEnv(ctx, ahrData, freq * 4.0f));
    int mod4 = m->addUgen(makeSinOscEnv(ctx, ahrData, freq * 8.0f));

    int modsSum = m->addUgen(new Sum(ctx, 4));

    int modMult = m->addUgen(new ConstMult(ctx, 6));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    carrier,    0,
            managerIn0, 1,    mod1,       0,
            managerIn0, 2,    mod2,       0,
            managerIn0, 3,    mod3,       0,
            managerIn0, 4,    mod4,       0,
            mod1,       0,    modsSum,    0,
            mod2,       0,    modsSum,    1,
            mod3,       0,    modsSum,    2,
            mod4,       0,    modsSum,    3,
            modsSum,    0,    modMult,    0,
            modMult,    0,    carrier,    1,
            carrier,    0,    MANAGER,    0,
            carrier,    1,    MANAGER,    1,
            carrier,    2,    MANAGER,    2
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnison(UgenCtx* ctx, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int managerIn0 = m->addUgen(new Split(ctx, 5));

    int osc1 = m->addUgen(makeOscEnvFM(ctx, freq));
    int osc2 = m->addUgen(makeOscEnvFM(ctx, freq + 0.2f));
    int osc3 = m->addUgen(makeOscEnvFM(ctx, freq - 0.2f));
    int osc4 = m->addUgen(makeOscEnvFM(ctx, freq + 0.4f));
    int osc5 = m->addUgen(makeOscEnvFM(ctx, freq - 0.4f));

    int oscsSum = m->addUgen(new Sum(ctx, 5));

    int mult = m->addUgen(new ConstMult(ctx, 0.2f));

    m->connect(
        std::vector<int>{
            MANAGER,    0,    managerIn0, 0,
            managerIn0, 0,    osc1,       0,
            managerIn0, 1,    osc2,       0,
            managerIn0, 2,    osc3,       0,
            managerIn0, 3,    osc4,       0,
            managerIn0, 4,    osc5,       0,
            osc1,       0,    oscsSum,    0,
            osc2,       0,    oscsSum,    1,
            osc3,       0,    oscsSum,    2,
            osc4,       0,    oscsSum,    3,
            osc5,       0,    oscsSum,    4,
            oscsSum,    0,    mult,       0,
            mult,       0,    MANAGER,    0,
            osc1,       1,    MANAGER,    1,
            osc1,       2,    MANAGER,    2
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnisonRecorder(UgenCtx* ctx, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int osc = m->addUgen(makeOscEnvFMUnison(ctx, freq));

    int audio = m->addUgen(new Split(ctx, 2));
    int env   = m->addUgen(new Split(ctx, 2));
    int onOff = m->addUgen(new Split(ctx, 3));

    int recorder1 = m->addUgen(
        "recorder1", 
        new Recorder(ctx, ahrData.getDurationSamps())
    );

    int recorder2 = m->addUgen(
        "recorder2", 
        new Recorder(ctx, ahrData.getDurationSamps())
    );

    m->connect(
        std::vector<int>{
            MANAGER, 0,    osc,       0,
            osc,     0,    audio,     0,
            osc,     1,    env,       0,
            osc,     2,    onOff,     0,
            audio,   0,    recorder1, 0,
            onOff,   0,    recorder1, 1,
            env,     0,    recorder2, 0,
            onOff,   1,    recorder2, 1,
            audio,   1,    MANAGER,   0,
            env,     1,    MANAGER,   1,
            onOff,   2,    MANAGER,   2
        }
    );

    return m;
}

}
