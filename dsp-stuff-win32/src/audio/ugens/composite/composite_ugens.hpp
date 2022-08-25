#pragma once

#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/wt_sin.hpp"

inline BaseUgen* makeOscEnv(double secondsPerSample) {
    double freq = 120;

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

    m->connectIn(0, ampEnv, 0);

    m->connectOut(ampVca, 0, 0);

    return m;
}
