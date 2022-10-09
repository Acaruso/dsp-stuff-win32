#pragma once

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/ahr_exp_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/split.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wavetable.hpp"
#include "src/audio/ugens/wt_sin.hpp"

// const float ampA = 1.0f;
// const float ampH = 200.0f;
// const float ampR = 200.0f;

const float ampA = 100.0f;
const float ampH = 200.0f;
const float ampR = 50.0f;

inline void connectSplitOut(UgenManager* m, int splitId, std::vector<int> destIds, int destPort) {
    int i = 0;
    for (auto destId : destIds) {
        m->connect(splitId, i++, destId, destPort);
    }
}

inline void connectSumIn(UgenManager* m, std::vector<int> sourceIds, int sourcePort, int sumId) {
    int i = 0;
    for (auto sourceId : sourceIds) {
        m->connect(sourceId, sourcePort, sumId, i++);
    }
}

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnv(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 2, 3);

    // create ugens
    int osc = m->addUgen(new WTSin(ugenCtx, freq));

    // int env = m->addUgen(new AHREnv(ugenCtx, ampA, ampH, ampR));
    // int env = m->addUgen(new AHRExpEnv(ugenCtx, ampA, ampH, ampR));

    int env = m->addUgen(
        new Wavetable(ugenCtx, &ugenCtx->wavetables.ahrEnv, mstosamps(350))
    );
    
    int env0split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(env, 0, env0split, 0);

    int vca = m->addUgen(new Mult(ugenCtx));

    // connect osc and env to vca
    m->connect(osc, 0, vca, 0);
    m->connect(env0split, 0, vca, 1);

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 2));
    m->connectIn(0, in0split, 0);
    m->connect(in0split, 0, env, 0);
    m->connect(in0split, 1, osc, 0);

    // in[1] - fm mod
    m->connectIn(1, osc, 1);

    // out[0] - audio
    m->connectOut(vca, 0, 0);

    // out[1] - amp env signal
    m->connectOut(env0split, 1, 1);

    // out[2] - amp env on/off
    m->connectOut(env, 1, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFM(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    // create ugens
    int carrier = m->addUgen(makeOscEnv(ugenCtx, freq));
    int mod1    = m->addUgen(makeOscEnv(ugenCtx, freq * 0.5f));
    int mod2    = m->addUgen(makeOscEnv(ugenCtx, freq * 2.0f));
    int mod3    = m->addUgen(makeOscEnv(ugenCtx, freq * 4.0f));
    int mod4    = m->addUgen(makeOscEnv(ugenCtx, freq * 8.0f));

    int modAmount = m->addUgen(new ConstValue(ugenCtx, 6));

    int modAmountMult = m->addUgen(new Mult(ugenCtx));

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 5));
    m->connectIn(0, in0split, 0);

    connectSplitOut(m, in0split, std::vector<int>{carrier, mod1, mod2, mod3, mod4}, 0);

    // sum mods, scale them by modAmount, connect them to carrier in
    int modsSum = m->addUgen(new Sum(ugenCtx, 4));

    connectSumIn(m, std::vector<int>{mod1, mod2, mod3, mod4}, 0, modsSum);

    m->connect(modsSum, 0, modAmountMult, 0);

    m->connect(modAmount, 0, modAmountMult, 1);

    m->connect(modAmountMult, 0, carrier, 1);

    // out[0] - audio
    m->connectOut(carrier, 0, 0);

    // out[1] - amp env signal
    m->connectOut(carrier, 1, 1);

    // out[2] - amp env on/off
    m->connectOut(carrier, 2, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnison(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    // create ugens
    int osc1 = m->addUgen(makeOscEnvFM(ugenCtx, freq));
    int osc2 = m->addUgen(makeOscEnvFM(ugenCtx, freq + 0.2f));
    int osc3 = m->addUgen(makeOscEnvFM(ugenCtx, freq - 0.2f));
    int osc4 = m->addUgen(makeOscEnvFM(ugenCtx, freq + 0.4f));
    int osc5 = m->addUgen(makeOscEnvFM(ugenCtx, freq - 0.4f));

    // in[0] - trig
    int in0split = m->addUgen(new Split(ugenCtx, 5));
    m->connectIn(0, in0split, 0);

    connectSplitOut(m, in0split, std::vector<int> {osc1, osc2, osc3, osc4, osc5}, 0);

    // sum oscs
    int oscsSum = m->addUgen(new Sum(ugenCtx, 5));

    connectSumIn(m, std::vector<int> {osc1, osc2, osc3, osc4, osc5}, 0, oscsSum);

    // turn down volume on oscs
    int mult = m->addUgen(new Mult(ugenCtx));

    int constValue = m->addUgen(new ConstValue(ugenCtx, 0.2f));

    m->connect(oscsSum, 0, mult, 0);

    m->connect(constValue, 0, mult, 1);

    // out[0] - audio
    m->connectOut(mult, 0, 0);

    // out[1] - amp env signal
    m->connectOut(osc1, 1, 1);

    // out[2] - amp env on/off
    m->connectOut(osc1, 2, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnisonRecorder(UgenCtx* ugenCtx, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);

    // create ugengs
    int osc = m->addUgen(makeOscEnvFMUnison(ugenCtx, freq));

    Recorder* pRecorder1 = new Recorder(ugenCtx);
    Recorder* pRecorder2 = new Recorder(ugenCtx);

    int recorder1 = m->addUgen("recorder1", pRecorder1);
    int recorder2 = m->addUgen("recorder2", pRecorder2);

    // resize recorder buffers
    unsigned envSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);

    pRecorder1->buffer.data.resize(envSamps, 0.0f);
    pRecorder2->buffer.data.resize(envSamps, 0.0f);

    // in[0] - trig
    m->connectIn(0, osc, 0);

    // split osc outs 0, 1, and 2
    int osc0split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 0, osc0split, 0);

    int osc1split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 1, osc1split, 0);

    int osc2split = m->addUgen(new Split(ugenCtx, 3));
    m->connect(osc, 2, osc2split, 0);

    // connect osc audio signal and osc on/off to recorder1
    m->connect(osc0split, 0, recorder1, 0);
    m->connect(osc2split, 0, recorder1, 1);

    // connect osc amp signal and osc on/off to recorder2
    m->connect(osc1split, 0, recorder2, 0);
    m->connect(osc2split, 1, recorder2, 1);

    // out[0] - audio
    m->connectOut(osc0split, 1, 0);

    // out[1] - amp env signal
    m->connectOut(osc1split, 1, 1);
    
    // out[2] - amp env on/off
    m->connectOut(osc2split, 2, 2);

    return m;
}
