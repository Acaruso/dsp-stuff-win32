#pragma once

#include <vector>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/waveform_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/shared/shared_data.hpp"

class CompositeFactory {
private:
    GraphicsService* gfx;
    InputState* inputState;
    SharedData* sharedData;

public:
    CompositeFactory(GraphicsService* _gfx, InputState* _inputState, SharedData* _sharedData) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
    }

    BaseElt* makeWaveContainer(std::vector<double>* buffer, RectWH rect) {
        RectWH containerRect = rect;
        RectWH waveRect = { 0, 0, rect.w, rect.h };

        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect), true);

        BaseElt* waveformElt = new WaveformElt(gfx, buffer, inputState, sharedData, makeRectF(waveRect));
        container->pushChild(waveformElt);

        return container;
    }

    BaseElt* makeWaveAndButton(std::vector<double>* buffer, RectWH rect) {
        int pad = 6;
        int buttonW = 40;
        int buttonH = 40;

        RectWH containerRect = rect;

        RectWH waveRect = {
            pad,
            pad,
            containerRect.w - (3 * pad) - buttonW,
            containerRect.h - (2 * pad)
        };

        RectWH buttonRect = {
            waveRect.x + waveRect.w + pad,
            waveRect.y,
            buttonW,
            buttonH
        };

        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect), true);

        container->pushChild(new RectElt(gfx, makeRectF(0, 0, rect.w, rect.h), blue, false, -1));

        BaseElt* wave = makeWaveContainer(buffer, waveRect);
        container->pushChild(wave);

        ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);
        button->onLeftClick = [sharedData = sharedData](int x, int y) { 
            sharedData->toAudio.enqueue("trig"); 
        };
        container->pushChild(button);

        return container;
    }
};
