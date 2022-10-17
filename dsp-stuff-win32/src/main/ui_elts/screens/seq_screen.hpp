#pragma once

#include "src/audio/ugens/basic_seq.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/composite/composite_ugens2.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/shared/shared_data.hpp"

class SeqScreen {
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
    ) {
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

        // create osc
        UgenManager* pOsc = makeOscEnv2(
            ugenCtx,
            AHRData{1.0f, 200.0f, 10.0f, 200.0f},
            AHRData{0.1f, 0.1f, 10.0f, 200.0f},
            60,
            400
        );

        // UgenManager* pOsc = makeOscEnv2(
        //     ugenCtx,
        //     AHRData{1.0f, 200.0f, 10.0f, 10.0f},
        //     AHRData{0.1f, 0.1f, 10.0f, 2.0f},
        //     60,
        //     400
        // );

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

        // create Play button
        BaseElt* playButton = uiCompositeFactory->makeButtonAndLabel(
            L"Play",
            960,
            20,
            [=](int x, int y) {
                rootUgenLock->lock();
                pSeq->toggle();
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(playButton);

        // create number
        BaseElt* number = uiCompositeFactory->makeNumberAndLabel(
            L"Period",
            &pSeq->period,
            0,
            10000,
            200,
            200
        );

        uiRoot->pushChild(number);

        rootUgenLock->unlock();
    }
};
