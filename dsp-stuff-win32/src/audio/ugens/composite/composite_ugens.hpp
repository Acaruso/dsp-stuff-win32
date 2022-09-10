#pragma once

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/const_value.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline BaseUgen* makeOscEnv(double _freq, double secondsPerSample) {
    double freq = _freq;

    double ampA = 1;
    double ampH = 200;
    double ampR = 200;

    UgenManager* m = new UgenManager();

    int osc = m->addUgen(new WTSin(secondsPerSample));
    ((WTSin*)m->getUgen(osc))->freq = freq;

    int ampEnv = m->addUgen(new AHREnv(ampA, ampH, ampR));

    int ampVca = m->addUgen(new Mult());

    m->connect(osc, 0, ampVca, 0);
    m->connect(ampEnv, 0, ampVca, 1);

    // trig
    m->connectIn(0, ampEnv, 0);

    // fm mod
    m->connectIn(1, osc, 0);

    m->connectOut(ampVca, 0, 0);

    m->connectOut(ampEnv, 0, 1);
    m->connectOut(ampEnv, 1, 2);

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline BaseUgen* makeOscEnvFM(double _freq, double secondsPerSample) {
    double freq = _freq;

    UgenManager* m = new UgenManager();

    int carrier = m->addUgen(makeOscEnv(freq, secondsPerSample));
    int mod1    = m->addUgen(makeOscEnv(freq * 0.5, secondsPerSample));
    int mod2    = m->addUgen(makeOscEnv(freq * 2, secondsPerSample));
    int mod3    = m->addUgen(makeOscEnv(freq * 4, secondsPerSample));
    int mod4    = m->addUgen(makeOscEnv(freq * 8, secondsPerSample));

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

    int gain  = m->addUgen(new ConstValue(1.0));
    int mult2 = m->addUgen(new Mult());

    m->connect(carrier, 0, mult2, 0);
    m->connect(gain, 0, mult2, 1);

    m->connectOut(mult2, 0, 0);

    m->connectOut(carrier, 1, 1);
    m->connectOut(carrier, 2, 2);

    return m;
}
