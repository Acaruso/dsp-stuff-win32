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

inline BaseElt* makeWaveContainer(
    GraphicsService* gfx, 
    std::vector<double>* buffer, 
    InputState* inputState,
    SharedData* sharedData,
    int x, 
    int y, 
    int w, 
    int h
) {
    RectWH containerRect = { x, y, w, h };
    RectWH waveRect = { 0, 0, w, h };

    BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect), true);

    BaseElt* waveformElt = new WaveformElt(gfx, buffer, inputState, sharedData, makeRectF(waveRect));
    container->pushChild(waveformElt);

    return container;
}

inline BaseElt* makeWaveAndButton(
    GraphicsService* gfx, 
    std::vector<double>* buffer, 
    InputState* inputState,
    SharedData* sharedData,
    int x, 
    int y, 
    int w, 
    int h
) {
    int pad = 6;
    int buttonW = 40;
    int buttonH = 40;

    RectWH containerRect = { x, y, w, h };

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

    container->pushChild(new RectElt(gfx, makeRectF(0, 0, w, h), blue, false, -1));

    BaseElt* wave = makeWaveContainer(
        gfx,
        buffer,
        inputState,
        sharedData, 
        waveRect.x, 
        waveRect.y, 
        waveRect.w, 
        waveRect.h
    );

    container->pushChild(wave);

    ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);
    button->onLeftClick = [=](int x, int y) { sharedData->toAudio.enqueue("trig"); };
    container->pushChild(button);

    return container;
}
