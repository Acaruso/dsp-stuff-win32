#pragma once

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"

// in[0]  - trig
// in[1]  - fm mod
// out[0] - audio
// out[1] - amp env on/off
// out[2] - amp env signal

inline BaseUgen* makeOscEnv(double _freq, double secondsPerSample) {
    double freq = _freq;

    double ampA = 1;
    double ampH = 200;
    double ampR = 500;

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
    m->connectOut(ampEnv, 1, 1);
    m->connectOut(ampEnv, 0, 2);

    return m;
}
