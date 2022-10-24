#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class SeqScreen : public BaseScreen {
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

    // kick drum-ish:

    void makeUgens() {
        rootUgenLock->lock();

        // create osc
        UgenManager* pOsc = makeSinOscEnvFreqEnv(
            ugenCtx,
            AHRData{1.0f, 200.0f, 10.0f},
            AHRData{0.1f, 0.1f, 10.0f},
            60,
            400
        );

        int osc = rootUgen->addUgen(pOsc);

        // create seq
        BasicSeq* pSeq = new BasicSeq(ugenCtx, 5000);
        int seq = rootUgen->addUgen(pSeq);

        // connect seq out0 to osc in0
        rootUgen->connect(seq, 0, osc, 0);

        // connect osc to outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);
        pOutSum->addIn();
        rootUgen->connect(osc, 0, outSum, numOscs);
        ++numOscs;

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

        // period number
        BaseElt* period = uiCompositeFactory->makeNumberAndLabel(
            L"Period",
            pSeq->period,
            0,
            100000,
            980,
            200,
            [=](int newNumber) { pSeq->period = newNumber; }
        );

        uiRoot->pushChild(period);

        // amp dur number
        WavetableEnv* pAmp = (WavetableEnv*)(pOsc->getUgen("ampEnv"));

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
        WavetableEnv* pFreq = (WavetableEnv*)(pOsc->getUgen("freqEnv"));

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

    // white noise:
    
    // void makeUgens() {
    //     rootUgenLock->lock();

    //     // create osc
    //     UgenManager* pOsc = makeWhiteNoiseOscEnv(
    //         ugenCtx,
    //         AHRData{1.0f, 200.0f, 10.0f}
    //     );

    //     int osc = rootUgen->addUgen(pOsc);

    //     // create seq
    //     BasicSeq* pSeq = new BasicSeq(ugenCtx, 5000);
    //     int seq = rootUgen->addUgen(pSeq);

    //     // connect seq out0 to osc in0
    //     rootUgen->connect(seq, 0, osc, 0);

    //     // connect osc to outSum
    //     int outSum = rootUgen->getUgenId("outSum");
    //     BaseUgen* pOutSum = rootUgen->getUgen(outSum);
    //     pOutSum->addIn();
    //     rootUgen->connect(osc, 0, outSum, numOscs);
    //     ++numOscs;

    //     // play button
    //     BaseElt* playButton = uiCompositeFactory->makeButtonAndLabel(
    //         L"Play",
    //         900,
    //         200,
    //         [=](int x, int y) {
    //             rootUgenLock->lock();
    //             pSeq->toggle();
    //             rootUgenLock->unlock();
    //         }
    //     );

    //     uiRoot->pushChild(playButton);

    //     // period number
    //     BaseElt* period = uiCompositeFactory->makeNumberAndLabel(
    //         L"Period",
    //         pSeq->period,
    //         0,
    //         100000,
    //         980,
    //         200,
    //         [=](int newNumber) { pSeq->period = newNumber; }
    //     );

    //     uiRoot->pushChild(period);

    //     // amp dur number
    //     WavetableEnv* pAmp = (WavetableEnv*)(pOsc->getUgen("ampEnv"));

    //     BaseElt* ampDur = uiCompositeFactory->makeNumberAndLabel(
    //         L"Amp Dur",
    //         sampstoms(pAmp->durationSamps),
    //         0,
    //         10000,
    //         200,
    //         300,
    //         [=](int newNumber) { pAmp->setDuration(newNumber); }
    //     );

    //     uiRoot->pushChild(ampDur);

    //     rootUgenLock->unlock();
    // }
};
