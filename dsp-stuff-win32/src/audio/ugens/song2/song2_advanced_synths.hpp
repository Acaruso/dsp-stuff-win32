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

struct AdvSynth1Params {
    AHRData wtAhr;
    AHRData modAhr;
    AHRData subAhr;
    AHRScaleData freqAhr;

    float wtFreq = 0.0f;
    float modFreq = 0.0f;
    float subFreq = 0.0f;

    float modAmount = 0.0f;
};

class AdvSynth1 : public BaseGen {
public:
    WavetableSynth wt;
    WavetableSynth mod;
    WavetableSynth sub;
    Waveshaper bitcrush;
    Waveshaper tanh;
    AdvancedSeq seq;
    Env freqEnv;
    OneShotSeq oneShotSeq;

    float modAmount = 1.0f;

    float sig = 0.0f;

    std::vector<AdvSynth1Params> params;
    int paramsCounter = 0;

    int curParam = 0;

    AdvSynth1(Wavetables* _wavetables, SeqClock* seqClock):
        wt(_wavetables->sin, AHRData{0, 10, 300}),
        mod(_wavetables->sin, AHRData{0, 10, 100}),
        sub(_wavetables->sin, AHRData{0, 10, 100}),
        bitcrush(_wavetables->bitcrush),
        tanh(_wavetables->tanh),
        seq(
            seqClock,
            //1           2           3           4
            { 1, 0, 0, 2, 3, 0, 0, 3, 1, 3, 3, 3, 1, 3, 2, 2 }
        ),
        oneShotSeq(
            200
        )
    {
        oneShotSeq.set32Note(0, 3);
        oneShotSeq.set32Note(1, 3);
        oneShotSeq.set32Note(2, 3);
        oneShotSeq.set32Note(3, 3);
        oneShotSeq.set32Note(0, 3);
        oneShotSeq.set32Note(1, 3);
        oneShotSeq.set32Note(2, 3);
        oneShotSeq.set32Note(3, 3);

        // dummy param
        AdvSynth1Params p0;
        params.push_back(p0);

        AdvSynth1Params p1;
        float h = 600;
        p1.wtAhr     = AHRData{0, h * 0.1f, h};
        p1.modAhr    = AHRData{0, h * 0.1f, h};
        p1.subAhr    = AHRData{0, h, h * 0.1f};
        p1.freqAhr   = AHRScaleData{1, 1, h, 50, 1500};
        p1.wtFreq    = 100.0f;
        p1.modFreq   = 50.0f;
        p1.subFreq   = 50.0f;
        p1.modAmount = 2.0f;
        params.push_back(p1);

        AdvSynth1Params p2;
        p2.wtAhr     = AHRData{0, h * 0.1f, h * 0.2f};
        p2.modAhr    = AHRData{0, h * 0.1f, h * 0.2f};
        p2.subAhr    = AHRData{0, h * 0.2f, h * 0.1f};
        p2.freqAhr   = AHRScaleData{1, 1, h * 0.2f, 100, 1500};
        p2.wtFreq    = 100.0f;
        p2.modFreq   = 50.0f;
        p2.subFreq   = 50.0f;
        p2.modAmount = 2.0f;
        params.push_back(p2);

        AdvSynth1Params p3;
        h = 20;
        p3.wtAhr     = AHRData{0, 1, h};
        p3.modAhr    = AHRData{0, 1, h};
        p3.subAhr    = AHRData{0, 1, h};
        p3.freqAhr   = AHRScaleData{1, 1, h, 400, 200};
        p3.wtFreq    = 400.0f;
        p3.modFreq   = 50.0f;
        p3.subFreq   = 50.0f;
        p3.modAmount = 6.0f;
        params.push_back(p3);
    }

    void setParams(AdvSynth1Params& p) {
        wt.setAmpEnv(p.wtAhr);
        mod.setAmpEnv(p.modAhr);
        sub.setAmpEnv(p.subAhr);
        freqEnv.setAhrScale(p.freqAhr);
        wt.setFreq(p.wtFreq);
        mod.setFreq(p.modFreq);
        sub.setFreq(p.subFreq);
        modAmount = p.modAmount;
    }

    float get() {
        sig = 0.0f;

        wt.setFreq(freqEnv.get());

        wt.setPhaseMod(
            tanh.get(mod.get() * 2.0f) * modAmount
        );

        sig += tanh.get(wt.get() * 0.4f) * 0.6f;
        sig += wt.get();
        sig += sub.get();

        return sig;
    }

    void trigger() {
        wt.trigger();
        mod.trigger();
        sub.trigger();
        freqEnv.trigger();
    }

    void run() override {
        if ((curParam = seq.trigger())) {
            setParams(params[curParam]);

            if (curParam == 3) {
                oneShotSeq.turnOn();
            } else {
                trigger();
            }
        }

        if (oneShotSeq.trigger()) {
            trigger();
        }

        wt.run();
        mod.run();
        sub.run();
        seq.run();
        freqEnv.run();
        oneShotSeq.run();
    }
};

}
