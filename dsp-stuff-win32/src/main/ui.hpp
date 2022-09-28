#pragma once

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/main/constants.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/composite/composites.hpp"
#include "src/shared/shared_data.hpp"

class Ui {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    CompositeFactory* compositeFactory = nullptr;

    int yInc = 250;
    RectWH oscRect = { 20, 20, 900, 200 };

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = new ContainerElt(gfx, makeRectF(0, 0, windowWidth, windowHeight));
        compositeFactory = new CompositeFactory(gfx, inputState, sharedData);
    }

    // complex //////////////////////////////////////////////////////

    void initUi() {
        makeOscUgenAndUi(oscRect, sharedData->rootUgenLock);
        oscRect.y += yInc;

        // button to add new ugen
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

        double freq = 120.0;

        UgenManager* pOsc = makeOscEnvFMUnisonRecorder(&sharedData->ugenCtx, freq);

        int osc = root->addUgen(pOsc);

        int outSink = root->getUgenId("outSink");

        root->connect(osc, 0, outSink, 0);

        rootUgenLock.unlock();

        // create osc ui elt
        uiRoot->pushChild(compositeFactory->makeTwoWavesAndButton(pOsc, oscRect));

        sharedData->ugenCtx.bufferAllocator.printAllocationMap();
    }

    // simple ///////////////////////////////////////////////////////

    // void initUi() {
    //     makeSimpleOscUgenAndUi(oscRect, sharedData->rootUgenLock);
    //     oscRect.y += yInc;

    //     // button to add new ugen
    //     RectWH buttonRect = { 960, 20, 40, 40 };

    //     ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);

    //     button->onLeftClick = [&](int x, int y) {
    //         makeSimpleOscUgenAndUi(oscRect, sharedData->rootUgenLock);
    //         oscRect.y += yInc;
    //     };

    //     uiRoot->pushChild(button);
    // }

    // void makeSimpleOscUgenAndUi(RectWH oscRect, std::mutex& rootUgenLock) {
    //     // create osc
    //     rootUgenLock.lock();

    //     UgenManager* root = &sharedData->rootUgen;

    //     double freq = 120.0;

    //     UgenManager* pOsc = makeOscEnv(&sharedData->ugenCtx, freq);

    //     int osc = root->addUgen(pOsc);

    //     int outSink = root->getUgenId("outSink");

    //     root->connect(osc, 0, outSink, 0);

    //     rootUgenLock.unlock();

    //     // button
    //     ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(oscRect), lightGray, gray);

    //     SharedData* pSharedData = sharedData;

    //     button->onLeftClick = [pSharedData = pSharedData, pOsc = pOsc](int x, int y) {
    //         ToAudioMessage message = { AM_TRIG, (uint64_t)pOsc, 0 };
    //         pSharedData->toAudio.enqueue(message);
    //     };

    //     uiRoot->pushChild(button);
    // }
};