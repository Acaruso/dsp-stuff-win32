#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/audio/ugens/seqs/pattern_seq.hpp"
#include "src/audio/ugens/seqs/value_seq.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/seq_grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class Song1Screen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    UgenManager* rootUgen = nullptr;
    UgenCtx* ugenCtx = nullptr;
    std::mutex* rootUgenLock;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState,
        BaseElt* _uiRoot,
        UiCompositeFactory* _uiCompositeFactory
    ) override {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = _uiRoot;
        uiCompositeFactory = _uiCompositeFactory;
        rootUgen = &sharedData->rootUgen;
        ugenCtx = rootUgen->ugenCtx;
        rootUgenLock = &sharedData->rootUgenLock;

        rootUgenLock->lock();
        makeUgens();
        makeUiControls();
        rootUgenLock->unlock();
    }

    void makeUgens() {
        // create kick
        UgenManager* pKick = makeSinOscEnvFreqEnv(
            ugenCtx,
            AHRData{0.0f, 200.0f, 10.0f},
            AHRData{0.0f, 1.0f, 100.0f},
            60,
            400,
            0.5f
        );

        int kick = rootUgen->addUgen("kick", pKick);

        // create white noise snare
        UgenManager* pSnare = makeWhiteNoiseOscEnv(
            ugenCtx,
            AHRData{1.0f, 80.0f, 180.0f},
            0.5f
        );

        int snare = rootUgen->addUgen("snare", pSnare);

        // // create bass
        // UgenManager* pBass = makeSinOscEnv(ugenCtx, AHRData{1.0f, 80.0f, 180.0f}, 100);
        // int bass = rootUgen->addUgen("bass", pBass);

        // create seq
        ValueSeq* pSeq = new ValueSeq(ugenCtx, 5000, 3);

        auto& d0 = pSeq->patterns[0].data;
        d0[0].on = true;
        d0[0].value = 1.0f;
        d0[2].on = true;
        d0[2].value = 1.0f;
        d0[4].on = true;
        d0[4].value = 1.0f;

        auto& d1 = pSeq->patterns[1].data;
        d1[3].on = true;
        d1[3].value = 1.0f;

        int seq = rootUgen->addUgen("seq", pSeq);

        // get outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);
        pOutSum->addIns(2);

        rootUgen->connect(
            std::vector<int> {
                seq,   0,    kick,   0,
                seq,   1,    snare,  0,
                kick,  0,    outSum, 0,
                snare, 0,    outSum, 1
            }
        );
    }

    void makeUiControls() {
        PatternSeq* pSeq = (PatternSeq*)rootUgen->getUgen("seq");
        
        // play button
        BaseElt* playButton = uiCompositeFactory->makeButtonAndLabel(
            L"Play",
            900,
            200,
            [=](int x, int y) {
                rootUgenLock->lock();
                pSeq->toggle();
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(playButton);

        // len16 number
        BaseElt* period = uiCompositeFactory->makeNumberAndLabel(
            L"Len16",
            pSeq->n16len,
            1,
            100000,
            980,
            200,
            [=](int newNumber) { pSeq->n16len = newNumber; }
        );

        uiRoot->pushChild(period);
    }
};
