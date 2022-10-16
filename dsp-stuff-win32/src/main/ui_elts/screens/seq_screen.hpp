#pragma once

#include "src/audio/ugens/basic_seq.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/composite/composite_ugens2.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
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
        AHRData ampEnvData  = { 1.0f, 200.0f, 10.0f, 200.0f };
        AHRData freqEnvData = { 0.1f, 0.1f, 10.0f, 200.0f };
        UgenManager* pOsc = makeOscEnv2(ugenCtx, ampEnvData, freqEnvData, 60, 400);
        int osc = rootUgen->addUgen(pOsc);

        // create seq
        int seq = rootUgen->addUgen(new BasicSeq(ugenCtx, 6000));

        // connect seq out0 to osc in0
        rootUgen->connect(seq, 0, osc, 0);

        // connect osc to outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);
        pOutSum->addIn();
        rootUgen->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        rootUgenLock->unlock();
    }
};
