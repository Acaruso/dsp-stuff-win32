#pragma once

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/ugen_data.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class SimpleScreen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiEltFactory* uiEltFactory = nullptr;

    int yInc = 250;
    RectWH oscRect = { 20, 20, 900, 200 };
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

        // create first oscillator
        makeSimpleOscUgenAndUi(oscRect, sharedData->rootUgenLock);
        oscRect.y += yInc;

        // create button to add additional oscillators
        ButtonElt* button = new ButtonElt(gfx, inputState, { 960, 20, 40, 40 }, lightGray, gray);

        button->onLeftClick = [&](int x, int y) {
            makeSimpleOscUgenAndUi(oscRect, sharedData->rootUgenLock);
            oscRect.y += yInc;
        };

        uiRoot->pushChild(button);
    }

    void makeSimpleOscUgenAndUi(RectWH oscRect, std::mutex& rootUgenLock) {
        // create osc
        rootUgenLock.lock();

        UgenManager* root = &sharedData->rootUgen;

        double freq = 120.0;

        UgenManager* pOsc = makeSinOscEnv(
            &sharedData->ugenCtx, 
            AHRData{100.0f, 200.0f, 50.0f},
            freq
        );

        int osc = root->addUgen(pOsc);

        BaseUgen* pBang = new Bang(&sharedData->ugenCtx);

        int bang = root->addUgen(pBang);

        root->connect(bang, 0, osc, 0);

        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        rootUgenLock.unlock();

        // button
        ButtonElt* button = new ButtonElt(gfx, inputState, oscRect, lightGray, gray);

        SharedData* pSharedData = sharedData;

        button->onLeftClick = [pSharedData = pSharedData, pBang = pBang](int x, int y) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            pSharedData->toAudio.enqueue(message);
        };

        uiRoot->pushChild(button);
    }
};
