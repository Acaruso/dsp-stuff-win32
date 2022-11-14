#pragma once

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ugen_ui_elt_factory.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class TwoOpTwoFreqEnvScreen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiEltFactory* uiEltFactory = nullptr;
    UgenUiEltFactory* ugenUiEltFactory = nullptr;

    int numOscs = 0;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState,
        BaseElt* _uiRoot
    ) override {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = _uiRoot;
        uiEltFactory = new UiEltFactory(gfx, inputState, sharedData);
        ugenUiEltFactory = new UgenUiEltFactory(gfx, sharedData, inputState );

        sharedData->rootUgenLock.lock();

        UgenManager* root = &sharedData->rootUgen;

        int osc = root->addUgen(
            makeTwoOpTwoFreqEnv(
                &sharedData->ugenCtx,
                { 0.0f, 100.0f, 50.0f, 0.0f, 1.0f },          // carrier amp
                { 150.0f, 100.0f, 50.0f, 0.0f, 16.0f },       // mod amp
                { 0.0f, 10.0f, 150.0f, 100.0f, 200.0f },      // carrier freq
                { 120.0f, 10.0f, 150.0f, 20.0f, 2000.0f }     // mod freq
            )
        );

        BaseUgen* pBang = new Bang(&sharedData->ugenCtx);
        int bang = root->addUgen(pBang);
        root->connect(bang, 0, osc, 0);

        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        BaseElt* button = ugenUiEltFactory->makeBangButton(pBang, 20, 20);
        uiRoot->pushChild(button);

        sharedData->rootUgenLock.unlock();
    }
};
