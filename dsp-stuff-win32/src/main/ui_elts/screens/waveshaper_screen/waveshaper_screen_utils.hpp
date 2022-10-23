#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/ugens/ahr_env.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/mult.hpp"
#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/scale.hpp"
#include "src/audio/ugens/split.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/audio/ugens/waveshaper.hpp"
#include "src/audio/ugens/wavetable_env.hpp"
#include "src/audio/ugens/wavetable_osc_freq_mod.hpp"

namespace WS {

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvWaveshaper(UgenCtx* ugenCtx, AHRData ampEnvData, float freq) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 3);
    UgenManager* pOsc = makeSinOscEnv(ugenCtx, ampEnvData, freq);
    int osc = m->addUgen(pOsc);
    int waveshaper = m->addUgen(new Waveshaper(ugenCtx, ugenCtx->wavetables.tanh));

    m->connect(
        std::vector<int> {
            MANAGER,    0,    osc,        0,
            osc,        0,    waveshaper, 0,
            waveshaper, 0,    MANAGER,    0,
            osc,        1,    MANAGER,    1,
            osc,        2,    MANAGER,    2
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio

inline UgenManager* addRecorders(UgenCtx* ugenCtx, UgenManager* pOsc, unsigned size) {
    UgenManager* m = new UgenManager(ugenCtx, 1, 1);

    int osc = m->addUgen(pOsc);
    m->connectIn(0, osc, 0);

    int osc0split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 0, osc0split, 0);

    int osc2split = m->addUgen(new Split(ugenCtx, 2));
    m->connect(osc, 2, osc2split, 0);

    int recorder1 = m->addUgen("recorder1", new Recorder(ugenCtx, size));
    int recorder2 = m->addUgen("recorder2", new Recorder(ugenCtx, size));

    m->connect(osc0split, 0, recorder1, 0);
    m->connect(osc2split, 0, recorder1, 1);

    m->connect(osc, 1, recorder2, 0);
    m->connect(osc2split, 1, recorder2, 1);

    m->connectOut(osc0split, 1, 0);

    return m;
}

}
