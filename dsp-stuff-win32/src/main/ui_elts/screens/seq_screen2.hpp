#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/audio/ugens/seqs/trigger_seq.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class SeqScreen2 : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    UgenManager* rootUgen = nullptr;
    UgenCtx* ugenCtx = nullptr;
    std::mutex* rootUgenLock;
    int numOscs = 0;

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

        makeUgens();
    }

    void makeUgens() {
        rootUgenLock->lock();

        // create kick
        UgenManager* pKick = makeSinOscEnvFreqEnvWTEnv(
            ugenCtx,
            AHRData{1.0f, 200.0f, 10.0f},
            AHRData{0.1f, 1.0f, 100.0f},
            60,
            400,
            0.5f
        );

        int kick = rootUgen->addUgen(pKick);

        // create white noise snare
        UgenManager* pSnare = makeWhiteNoiseOscEnvWTEnv(
            ugenCtx,
            AHRData{1.0f, 80.0f, 180.0f},
            0.5f
        );

        int snare = rootUgen->addUgen(pSnare);

        // create seq
        TriggerSeq* pSeq = new TriggerSeq(ugenCtx, 5000, 2);
        int seq = rootUgen->addUgen(pSeq);

        // connect seq out0 to kick in0
        rootUgen->connect(seq, 0, kick, 0);

        // connect seq out1 to snare in0
        rootUgen->connect(seq, 1, snare, 0);

        // get outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);

        // connect osc to outSum
        pOutSum->addIn();
        rootUgen->connect(kick, 0, outSum, numOscs);
        ++numOscs;

        // connect snare to outSum
        pOutSum->addIn();
        rootUgen->connect(snare, 0, outSum, numOscs);
        ++numOscs;

        // ui elements //////////////////////////////////////////////////////////

        // create play button
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

        // create len16 number
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

        // amp dur number
        WavetableEnv* pAmp = (WavetableEnv*)(pKick->getUgen("ampEnv"));

        BaseElt* ampDur = uiCompositeFactory->makeNumberAndLabel(
            L"Amp Dur",
            sampstoms(pAmp->durationSamps),
            0,
            10000,
            200,
            300,
            [=](int newNumber) { pAmp->setDuration(newNumber); }
        );

        uiRoot->pushChild(ampDur);

        // freq dur number
        WavetableEnv* pFreq = (WavetableEnv*)(pKick->getUgen("freqEnv"));

        BaseElt* freqDur = uiCompositeFactory->makeNumberAndLabel(
            L"Freq Dur",
            sampstoms(pFreq->durationSamps),
            0,
            10000,
            280,
            300,
            [=](int newNumber) { pFreq->setDuration(newNumber); }
        );

        uiRoot->pushChild(freqDur);

        rootUgenLock->unlock();
    }
};
