#pragma once

#include <cmath>
#include <iostream>
#include <variant>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/audio/ugens/song2/song2_env.hpp"
#include "src/audio/ugens/song2/song2_freqs.hpp"
#include "src/audio/ugens/song2/song2_note_util.hpp"
#include "src/audio/ugens/song2/song2_oscs.hpp"
#include "src/audio/ugens/song2/song2_seq.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/shared/shared_constants.hpp"

// out[0] - audio signal

namespace Song2 {

// inline std::variant<PolyWavetable, Env> Gen;

using Gen = std::variant<PolyWavetable, SawOp, Env>;
inline auto v_run = [](auto& gen) { gen.run(); };

class Gens {
public:
    std::vector<Gen> v;

    template<typename T>
    int push() {
        v.push_back(T{});
        return v.size() - 1;
    }

    template<typename T>
    T& get(int i) {
        return std::get<T>(
            v[i]
        );
    }
};

class Main : public BaseUgen {
public:
    Notes noteUtil;
    Seq seq;

    Saw saw;
    Square square;
    Triangle triangle{1.0f};
    Wavetable wt;
    Wavetable wtMod;

    Env env{AHRData{1.0f, 200.0f, 10000.0f}};
    Env envMod{AHRData{1.0f, 20.0f, 50.0f}};

    // PolyWavetable polyWt;

    // Gen v_polyWt = PolyWavetable{};
    // PolyWavetable& polyWt = std::get<PolyWavetable>(v_polyWt);

    // PolyWavetable* polyWt = nullptr;
    // SawOp* sawOp = nullptr;

    int polyWt;
    int sawOp;

    Seq sawOpSeq;
    int sawOpCounter = 0;

    std::vector<Freqs> chordProg2 = {
        noteUtil.makeMajorChord(0),
        noteUtil.makeMinorChord(9),
        noteUtil.makeMajorChord(5),
        noteUtil.makeMajorChord(7),
        noteUtil.makeMajorChord(0),
        noteUtil.makeMinorChord(9),
        noteUtil.makeMajorChord(5),
        noteUtil.makeMajorChord(7),
    };

    float wtSig = 0.0f;
    float outSig = 0.0f;

    // std::vector<Gen> gens;
    Gens gens;

    Main(
        UgenCtx* _ugenCtx,
        std::vector<float>* _wavetable,
        float _level=1.0f
    ) {
        typeStr = "Song2::Main";
        ugenCtx = _ugenCtx;
        level = _level;
        numIns = 0;
        numOuts = 1;
        allocateBuffers(typeStr);

        // gens.push_back(PolyWavetable{});
        // polyWt = &(std::get<PolyWavetable>(gens.back()));

        polyWt = gens.push<PolyWavetable>();

        // gens.push_back(SawOp{});
        // sawOp = &(std::get<SawOp>(gens.back()));

        wt.setWavetable(ugenCtx->wavetables.sin);
        wtMod.setWavetable(ugenCtx->wavetables.sin);

        // polyWt->setWavetable(ugenCtx->wavetables.sin);
        // polyWt->setEnv(AHRData{1.0f, 100.0f, 600.0f});
        // polyWt->setEnvMod(AHRData{1.0f, 20.0f, 500.0f});
        // polyWt->setModAmount(16.0f);

        gens.get<PolyWavetable>(polyWt).setWavetable(ugenCtx->wavetables.sin);
        gens.get<PolyWavetable>(polyWt).setEnv(AHRData{1.0f, 100.0f, 600.0f});
        gens.get<PolyWavetable>(polyWt).setEnvMod(AHRData{1.0f, 20.0f, 500.0f});
        gens.get<PolyWavetable>(polyWt).setModAmount(16.0f);

        // sawOp->setEnv(AHRData{1.0f, 50.0f, 100.0f});
        // sawOpSeq.setOneBarPattern(
        //     //                1           2           3           4
        //     std::vector<int>{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0 }
        // );

        // gens.push_back(v_polyWt);
        // gens.push_back(v_sawOp);
    }

    void run(unsigned sampleCounter) override {
        auto& d = ugenCtx->bufferAllocator.data;
        unsigned out0 = out[0];

        for (int i = 0; i < bufferSize; ++i) {
            if (seq.trigger()) {
                gens.get<PolyWavetable>(polyWt).setFreqs(chordProg2[seq.measures]);
                gens.get<PolyWavetable>(polyWt).trigger();
            }

            // if (sawOpSeq.trigger()) {
            //     Freqs curFreqs = chordProg2[seq.measures];
            //     float sawOpFreq = curFreqs.f[sawOpCounter];
            //     sawOpCounter = (sawOpCounter + 1) % 3;

            //     sawOp->setFreq(sawOpFreq);
            //     sawOp->trigger();
            // }

            wtSig = gens.get<PolyWavetable>(polyWt).get();

            // outSig = (wtSig * 0.5) + (sawOp->get() * 0.17);

            outSig = (wtSig * 0.5);

            WRITE_OUT(d, out0, i, outSig);

            runAll();
        }
    }

    void runAll() {
        saw.run();
        square.run();
        triangle.run();
        wt.run();
        wtMod.run();
        env.run();
        envMod.run();

        // polyWt.run();
        // sawOp->run();

        // std::visit(v_run, v_polyWt);
        // std::visit(v_run, v_sawOp);

        for (auto& gen : gens.v) {
            std::visit(v_run, gen);
        }

        seq.run();
        sawOpSeq.run();
    }
};

}
