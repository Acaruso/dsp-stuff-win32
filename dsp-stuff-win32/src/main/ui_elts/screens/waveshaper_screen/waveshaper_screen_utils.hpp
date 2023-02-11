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

UgenManager* makeOscEnvWaveshaper(UgenCtx* ctx, std::vector<float>* wavetable, AHRData ampEnvData, float freq);

// in[0]  - trig
// out[0] - audio

inline UgenManager* makeOscEnvWaveshaperRecorders(UgenCtx* ctx, std::vector<float>* wavetable, AHRData ampEnvData, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 1);

    int osc = m->addUgen(makeOscEnvWaveshaper(ctx, wavetable, ampEnvData, freq));

    int oscOut0 = m->addUgen(new Split(ctx, 2));

    int oscOut2 = m->addUgen(new Split(ctx, 2));

    int recorder1 = m->addUgen(
        "recorder1", 
        new Recorder(ctx, ampEnvData.getDurationSamps())
    );

    int recorder2 = m->addUgen(
        "recorder2", 
        new Recorder(ctx, ampEnvData.getDurationSamps())
    );

    m->connect(
        std::vector<int>{
            MANAGER, 0,    osc,       0,
            osc,     0,    oscOut0,   0,
            osc,     2,    oscOut2,   0,
            oscOut0, 0,    recorder1, 0,
            oscOut2, 0,    recorder1, 1,
            osc,     1,    recorder2, 0,
            oscOut2, 1,    recorder2, 1,
            oscOut0, 1,    MANAGER,   0
        }
    );

    return m;
}

// in[0]  - trig
// out[0] - audio
// out[1] - amp env signal
// out[2] - amp env on/off

inline UgenManager* makeOscEnvWaveshaper(UgenCtx* ctx, std::vector<float>* wavetable, AHRData ampEnvData, float freq) {
    UgenManager* m = new UgenManager(ctx, 1, 3);
    UgenManager* pOsc = makeSinOscEnv(ctx, ampEnvData, freq);
    int osc = m->addUgen(pOsc);

    int waveshaper = m->addUgen(new Waveshaper(ctx, wavetable));

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

}
