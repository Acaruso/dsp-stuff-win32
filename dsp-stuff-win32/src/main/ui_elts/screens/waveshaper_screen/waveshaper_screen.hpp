#pragma once

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/main/ui_elts/screens/waveshaper_screen/waveshaper_screen_utils.hpp"
#include "src/shared/shared_data.hpp"

class WaveshaperScreen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiEltFactory* uiEltFactory = nullptr;

    int yInc = 250;
    RectWH oscRect = { 20, 20, 900, 200 };
    int numOscs = 0;

    std::vector<float>* wavetable;

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

        wavetable = sharedData->ugenCtx.wavetables.saw;

        // create first oscillator
        makeOscUgenAndUi(wavetable, oscRect, sharedData->rootUgenLock);
        oscRect.y += yInc;

        // create button to add additional oscillators
        ButtonElt* button = new ButtonElt(gfx, inputState, { 960, 20, 40, 40 }, lightGray, gray);

        button->onLeftClick = [&](int x, int y) {
            makeOscUgenAndUi(wavetable, oscRect, sharedData->rootUgenLock);
            oscRect.y += yInc;
        };

        uiRoot->pushChild(button);

        // display waveshaper
        SharedAudioBuffer* buf = new SharedAudioBuffer{
            *wavetable,
            false
        };

        BaseElt* waveshaperDisplay = uiEltFactory->makeWaveContainer(
            buf,
            { 1100, 20, 200, 200 }
        );

        uiRoot->pushChild(waveshaperDisplay);
    }

    void makeOscUgenAndUi(std::vector<float>* wavetable, RectWH oscRect, std::mutex& rootUgenLock) {
        rootUgenLock.lock();

        UgenManager* root = &sharedData->rootUgen;
        UgenCtx* ugenCtx = root->ugenCtx;

        // create osc
        double freq = 50.0;

        UgenManager* pOsc = WS::makeOscEnvWaveshaperRecorders(
            ugenCtx,
            wavetable,
            AHRData{10.0f, 200.0f, 10.0f},
            freq
        );
        int osc = root->addUgen(pOsc);

        // create bang and connect to osc in0
        BaseUgen* pBang = new Bang(ugenCtx);
        int bang = root->addUgen(pBang);
        root->connect(bang, 0, osc, 0);

        // connect osc to outSum
        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(osc, 0, outSum, numOscs);
        ++numOscs;

        rootUgenLock.unlock();

        // create osc ui elt
        uiRoot->pushChild(uiEltFactory->makeTwoWavesAndButton(pOsc, pBang, oscRect));
    }
};
