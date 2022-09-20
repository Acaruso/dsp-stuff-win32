#pragma once

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"

const float ampA = 1.0f;
const float ampH = 200.0f;
const float ampR = 200.0f;

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnv(float freq) {
    UgenManager* m = new UgenManager();

    int osc = m->addUgen(new WTSin());
    ((WTSin*)m->getUgen(osc))->freq = freq;

    int ampEnv = m->addUgen(new AHREnv(ampA, ampH, ampR));

    int ampVca = m->addUgen(new Mult());

    m->connect(osc, 0, ampVca, 0);
    m->connect(ampEnv, 0, ampVca, 1);

    // env trigger
    m->connectIn(0, ampEnv, 0);

    // osc phase reset
    m->connectIn(0, osc, 0);

    // fm mod
    m->connectIn(1, osc, 1);

    m->connectOut(ampVca, 0, 0);

    m->connectOut(ampEnv, 0, 1);
    m->connectOut(ampEnv, 1, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFM(float freq) {
    UgenManager* m = new UgenManager();

    int carrier = m->addUgen(makeOscEnv(freq));
    int mod1    = m->addUgen(makeOscEnv(freq * 0.5f));
    int mod2    = m->addUgen(makeOscEnv(freq * 2.0f));
    int mod3    = m->addUgen(makeOscEnv(freq * 4.0f));
    int mod4    = m->addUgen(makeOscEnv(freq * 8.0f));

    int constValue = m->addUgen(new ConstValue(6));
    int mult       = m->addUgen(new Mult());

    // trigs
    m->connectIn(0, carrier, 0);
    m->connectIn(0, mod1, 0);
    m->connectIn(0, mod2, 0);
    m->connectIn(0, mod3, 0);
    m->connectIn(0, mod4, 0);

    m->connect(constValue, 0, mult, 0);
    m->connect(mod1, 0, mult, 1);
    m->connect(mod2, 0, mult, 1);
    m->connect(mod3, 0, mult, 1);
    m->connect(mod4, 0, mult, 1);
    m->connect(mult, 0, carrier, 1);

    int gain  = m->addUgen(new ConstValue(1.0f));
    int mult2 = m->addUgen(new Mult());

    m->connect(carrier, 0, mult2, 0);
    m->connect(gain, 0, mult2, 1);

    m->connectOut(mult2, 0, 0);

    m->connectOut(carrier, 1, 1);
    m->connectOut(carrier, 2, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnison(float freq) {
    UgenManager* m = new UgenManager;

    int osc1 = m->addUgen(makeOscEnvFM(freq));
    int osc2 = m->addUgen(makeOscEnvFM(freq + 0.2f));
    int osc3 = m->addUgen(makeOscEnvFM(freq - 0.2f));
    int osc4 = m->addUgen(makeOscEnvFM(freq + 0.4f));
    int osc5 = m->addUgen(makeOscEnvFM(freq - 0.4f));

    m->connectIn(0, osc1, 0);
    m->connectIn(0, osc2, 0);
    m->connectIn(0, osc3, 0);
    m->connectIn(0, osc4, 0);
    m->connectIn(0, osc5, 0);

    int mult = m->addUgen(new Mult());
    int constValue = m->addUgen(new ConstValue(0.2f));

    m->connect(osc1, 0, mult, 0);
    m->connect(osc2, 0, mult, 0);
    m->connect(osc3, 0, mult, 0);
    m->connect(osc4, 0, mult, 0);
    m->connect(osc5, 0, mult, 0);

    m->connect(constValue, 0, mult, 1);

    m->connectOut(mult, 0, 0);
    m->connectOut(osc1, 1, 1);
    m->connectOut(osc1, 2, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvFMUnisonRecorder(float freq) {
    UgenManager* m = new UgenManager;

    int osc = m->addUgen(makeOscEnvFMUnison(freq));

    Recorder* pRecorder1 = new Recorder;
    Recorder* pRecorder2 = new Recorder;

    int recorder1 = m->addUgen("recorder1", pRecorder1);
    int recorder2 = m->addUgen("recorder2", pRecorder2);

    unsigned envSamps = mstosamps(ampA) + mstosamps(ampH) + mstosamps(ampR);

    pRecorder1->buffer.data.resize(envSamps, 0.0f);
    pRecorder2->buffer.data.resize(envSamps, 0.0f);

    m->connect(osc, 0, recorder1, 0);
    m->connect(osc, 2, recorder1, 1);

    m->connect(osc, 1, recorder2, 0);
    m->connect(osc, 2, recorder2, 1);

    m->connectIn(0, osc, 0);

    m->connectOut(osc, 0, 0);
    m->connectOut(osc, 1, 1);
    m->connectOut(osc, 2, 2);

    return m;
}
