#pragma once

#include <vector>

#include "src/audio/audio_constants.hpp"
#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/song2/song2_base_gen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/song2/song2_synths.hpp"
#include "src/audio/ugens/song2/song2_waveshaper.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/main/util.hpp"

namespace Song2 {

class AdvSynth1 : public BaseGen {
public:
    WavetableSynth wt;
    WavetableSynth mod;
    WavetableSynth sub;
    Waveshaper bitcrush;
    Waveshaper tanh;
    AdvancedSeq seq;

    float sig = 0.0f;

    AdvSynth1(Wavetables* _wavetables, SeqClock* seqClock):
        wt(_wavetables->sin, AHRData{0, 10, 300}),
        mod(_wavetables->sin, AHRData{0, 10, 100}),
        sub(_wavetables->sin, AHRData{0, 10, 100}),
        bitcrush(_wavetables->bitcrush),
        tanh(_wavetables->tanh),
        seq(
            seqClock,
            //1           2           3           4
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        )
    {
        float h = 600;

        wt.setAmpEnv(AHRData {0, h * 0.1f, h * 0.3f});
        mod.setAmpEnv(AHRData{0, h * 0.1f, h * 0.15f});
        sub.setAmpEnv(AHRData{0, h, 1});

        wt.setFreq(100);
        mod.setFreq(50);
        sub.setFreq(50);
    }

    float get() {
        sig = 0.0f;
        wt.setPhaseMod(mod.get() * 1.0f);

        // return tanh.get(
        //     bitcrush.get(
        //         wt.get()
        //     ) * 4.0f
        // );

        // return bitcrush.get(
        //     tanh.get(
        //         wt.get() * 4.0f
        //     )
        // );

        sig += tanh.get(wt.get() + (sub.get() * 0.06f));
        sig += sub.get();

        return sig;

        // return wt.get();
    }

    void run() override {
        if (seq.trigger()) {
            wt.trigger();
            mod.trigger();
            sub.trigger();
        }
        wt.run();
        mod.run();
        sub.run();
        seq.run();
    }
};

}
