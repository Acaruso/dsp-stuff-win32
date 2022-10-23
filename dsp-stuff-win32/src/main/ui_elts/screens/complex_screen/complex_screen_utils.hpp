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

const AHRData ahrData = { 100.0f, 200.0f, 50.0f, 350.0f };

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFM(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    int managerIn0 = m->addUgen(new Split(ugenCtx, 5));

    int carrier = m->addUgen(makeSinOscEnv(ugenCtx, ahrData, freq));

    int mod1 = m->addUgen(makeSinOscEnv(ugenCtx, ahrData, freq * 0.5f));
    int mod2 = m->addUgen(makeSinOscEnv(ugenCtx, ahrData, freq * 2.0f));
    int mod3 = m->addUgen(makeSinOscEnv(ugenCtx, ahrData, freq * 4.0f));
    int mod4 = m->addUgen(makeSinOscEnv(ugenCtx, ahrData, freq * 8.0f));

    int modsSum = m->addUgen(new Sum(ugenCtx, 4));

    int modAmount = m->addUgen(new ConstValue(ugenCtx, 6));

    int modAmountMult = m->addUgen(new Mult(ugenCtx));

    m->connect(
        std::vector<int>{
            MANAGER,       0,    managerIn0,    0,
            managerIn0,    0,    carrier,       0,
            managerIn0,    1,    mod1,          0,
            managerIn0,    2,    mod2,          0,
            managerIn0,    3,    mod3,          0,
            managerIn0,    4,    mod4,          0,
            mod1,          0,    modsSum,       0,
            mod2,          0,    modsSum,       1,
            mod3,          0,    modsSum,       2,
            mod4,          0,    modsSum,       3,
            modAmount,     0,    modAmountMult, 0,
            modsSum,       0,    modAmountMult, 1,
            modAmountMult, 0,    carrier,       1,
            carrier,       0,    MANAGER,       0,
            carrier,       1,    MANAGER,       1,
            carrier,       2,    MANAGER,       2
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnison(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    int managerIn0 = m->addUgen(new Split(ugenCtx, 5));

    int osc1 = m->addUgen(makeOscEnvFM(ugenCtx, freq));
    int osc2 = m->addUgen(makeOscEnvFM(ugenCtx, freq + 0.2f));
    int osc3 = m->addUgen(makeOscEnvFM(ugenCtx, freq - 0.2f));
    int osc4 = m->addUgen(makeOscEnvFM(ugenCtx, freq + 0.4f));
    int osc5 = m->addUgen(makeOscEnvFM(ugenCtx, freq - 0.4f));

    int oscsSum = m->addUgen(new Sum(ugenCtx, 5));

    int mult = m->addUgen(new ConstMult(ugenCtx, 0.2f));

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

// inline UgenManager* makeOscEnvFMUnisonRecorder(UgenCtx* ugenCtx, float freq) {
//     UgenManager* m = new UgenManager(ugenCtx, 1, 3);

//     // create ugengs
//     int osc = m->addUgen(makeOscEnvFMUnison(ugenCtx, freq));

//     Recorder* pRecorder1 = new Recorder(ugenCtx);
//     Recorder* pRecorder2 = new Recorder(ugenCtx);

//     int recorder1 = m->addUgen("recorder1", pRecorder1);
//     int recorder2 = m->addUgen("recorder2", pRecorder2);

//     // resize recorder buffers
//     unsigned envSamps = mstosamps(ahrData.a) + mstosamps(ahrData.h) + mstosamps(ahrData.r);

//     pRecorder1->buffer.data.resize(envSamps, 0.0f);
//     pRecorder2->buffer.data.resize(envSamps, 0.0f);

//     // in[0] - trig
//     m->connectIn(0, osc, 0);

//     // split osc outs 0, 1, and 2
//     int osc0split = m->addUgen(new Split(ugenCtx, 2));
//     m->connect(osc, 0, osc0split, 0);

//     int osc1split = m->addUgen(new Split(ugenCtx, 2));
//     m->connect(osc, 1, osc1split, 0);

//     int osc2split = m->addUgen(new Split(ugenCtx, 3));
//     m->connect(osc, 2, osc2split, 0);

//     // connect osc audio signal and osc on/off to recorder1
//     m->connect(osc0split, 0, recorder1, 0);
//     m->connect(osc2split, 0, recorder1, 1);

//     // connect osc amp signal and osc on/off to recorder2
//     m->connect(osc1split, 0, recorder2, 0);
//     m->connect(osc2split, 1, recorder2, 1);

//     // out[0] - audio
//     m->connectOut(osc0split, 1, 0);

//     // out[1] - amp env signal
//     m->connectOut(osc1split, 1, 1);
    
//     // out[2] - amp env on/off
//     m->connectOut(osc2split, 2, 2);

//     return m;
// }

inline UgenManager* makeOscEnvFMUnisonRecorder(UgenCtx* ctx, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 3);

    int osc = m->addUgen(makeOscEnvFMUnison(ctx, freq));

    int oscAudio = m->addUgen(new Split(ctx, 2));
    int oscEnv   = m->addUgen(new Split(ctx, 2));
    int oscOnOff = m->addUgen(new Split(ctx, 3));

    int recorder1 = m->addUgen(
        "recorder1", 
        new Recorder(ctx, mstosamps(ahrData.duration))
    );

    int recorder2 = m->addUgen(
        "recorder2", 
        new Recorder(ctx, mstosamps(ahrData.duration))
    );

    m->connect(
        std::vector<int>{
            MANAGER,  0,    osc,       0,
            osc,      0,    oscAudio,  0,
            osc,      1,    oscEnv,    0,
            osc,      2,    oscOnOff,  0,
            oscAudio, 0,    recorder1, 0,
            oscOnOff, 0,    recorder1, 1,
            oscEnv,   0,    recorder2, 0,
            oscOnOff, 1,    recorder2, 1,
            oscAudio, 1,    MANAGER,   0,
            oscEnv,   1,    MANAGER,   1,
            oscOnOff, 2,    MANAGER,   2
        }
    );

    return m;
}

}
