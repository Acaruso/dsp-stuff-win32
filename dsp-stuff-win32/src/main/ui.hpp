#pragma once

#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/main/constants.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/composite/composites.hpp"
#include "src/main/ui_elts/ui_elt_util.hpp"
#include "src/shared/shared_data.hpp"

class Ui {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    CompositeFactory* uiCompositeFactory = nullptr;

    int yInc = 250;
    RectWH oscRect = { 20, 20, 900, 200 };

    int numOscs = 0;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = new ContainerElt(gfx, makeRectF(0, 0, windowWidth, windowHeight));
        uiCompositeFactory = new CompositeFactory(gfx, inputState, sharedData);
    }

    void initUi() {
        // initUiComplex();
        initUiSimple();
    }

    // complex //////////////////////////////////////////////////////

    void initUiComplex() {
        // create first oscillator
        makeOscUgenAndUi(oscRect, sharedData->rootUgenLock);
        oscRect.y += yInc;

        // create button to add additional oscillators
        RectWH buttonRect = { 960, 20, 40, 40 };

        ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);

        button->onLeftClick = [&](int x, int y) {
            makeOscUgenAndUi(oscRect, sharedData->rootUgenLock);
            oscRect.y += yInc;
        };

        uiRoot->pushChild(button);
    }

    void makeOscUgenAndUi(RectWH oscRect, std::mutex& rootUgenLock) {
        // create osc
        rootUgenLock.lock();

        UgenManager* root = &sharedData->rootUgen;
        UgenCtx* ugenCtx = root->ugenCtx;

        double freq = 120.0;

        UgenManager* pOsc = makeOscEnvFMUnisonRecorder(ugenCtx, freq);

        int osc = root->addUgen(pOsc);

        BaseUgen* pBang = new Bang(ugenCtx);

        int bang = root->addUgen(pBang);

        root->connect(bang, 0, osc, 0);

        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        rootUgenLock.unlock();

        // create osc ui elt
        uiRoot->pushChild(uiCompositeFactory->makeTwoWavesAndButton(pOsc, pBang, oscRect));
    }

    // simple ///////////////////////////////////////////////////////

    void initUiSimple() {
        makeSimpleOscUgenAndUi(oscRect, sharedData->rootUgenLock);
        oscRect.y += yInc;

        // button to add new ugen
        RectWH buttonRect = { 960, 20, 40, 40 };

        ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);

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

        UgenManager* pOsc = makeOscEnv(&sharedData->ugenCtx, freq);
        // UgenManager* pOsc = makeOscEnvFMUnisonRecorder(&sharedData->ugenCtx, freq);

        int osc = root->addUgen(pOsc);

        BaseUgen* pBang = new Bang(&sharedData->ugenCtx);

        int bang = root->addUgen(pBang);

        root->connect(bang, 0, osc, 0);

        int outSink = root->getUgenId("outSink");

        root->connect(osc, 0, outSink, 0);

        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        rootUgenLock.unlock();

        // button
        ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(oscRect), lightGray, gray);

        SharedData* pSharedData = sharedData;

        // button->onLeftClick = [pSharedData = pSharedData, pOsc = pOsc](int x, int y) {
        //     ToAudioMessage message = { AM_TRIG, (uint64_t)pOsc, 0 };
        //     pSharedData->toAudio.enqueue(message);
        // };

        button->onLeftClick = [pSharedData = pSharedData, pBang = pBang](int x, int y) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            pSharedData->toAudio.enqueue(message);
        };

        uiRoot->pushChild(button);
    }

    void handleLeftClick(int x, int y) {
        ::handleLeftClick(uiRoot, x, y);
    }

    void handleLeftDrag(int x, int y, int xDelta, int yDelta) {
        ::handleLeftDrag(uiRoot, x, y, xDelta, yDelta);
    }

    void handleMouseWheel(int wheelDelta) {
        ::handleMouseWheel(uiRoot, inputState, wheelDelta);
    }

    void handleKeyDown(int keyCode) {
        ::handleKeyDown(uiRoot, inputState, keyCode);
    }

    void handleDraw() {
        ::handleDraw(gfx, uiRoot);
    }

    void handleTick() {
        ::handleTick(uiRoot);
    }
};
