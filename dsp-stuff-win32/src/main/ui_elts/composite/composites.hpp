#pragma once

#include <vector>

#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
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

    BaseElt* makeWaveContainer(SharedBuffer* buffer, RectWH rect) {
        RectWH containerRect = rect;
        RectWH waveRect = { 0, 0, rect.w, rect.h };

        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect), true);

        BaseElt* waveformElt = new WaveformElt(
            gfx,
            buffer,
            inputState,
            sharedData,
            makeRectF(waveRect)
        );

        container->pushChild(waveformElt);

        return container;
    }

    BaseElt* makeWaveAndButton(SharedBuffer* buffer, RectWH rect) {
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
            ToAudioMessage message;
            message.type = AM_TRIG;
            sharedData->toAudio.enqueue(message);
        };
        container->pushChild(button);

        return container;
    }

    BaseElt* makeTwoWavesAndButton(UgenManager* osc, RectWH containerRect) {
        int padding = 6;
        int buttonW = 40;
        int buttonH = 40;

        // outer container
        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect), true);

        // outer container background
        RectWH bgRect = { 0, 0, containerRect.w, containerRect.h };
        container->pushChild(new RectElt(gfx, makeRectF(bgRect), blue, false, -1));

        // wave 1
        Recorder* recorder1 = (Recorder*)osc->getUgen("recorder1");

        RectWH innerRect = { 
            padding,
            padding,
            containerRect.w - ((padding * 3) + buttonW),
            (containerRect.h - (padding * 3)) / 2
        };

        container->pushChild(makeWaveContainer(&recorder1->buffer, innerRect));

        // wave 2
        Recorder* recorder2 = (Recorder*)osc->getUgen("recorder2");

        innerRect.y += innerRect.h + padding;

        container->pushChild(makeWaveContainer(&recorder2->buffer, innerRect));

        // button
        RectWH buttonRect = {
            padding + innerRect.w + padding,
            padding,
            buttonW,
            buttonH
        };

        ButtonElt* button = new ButtonElt(gfx, inputState, makeRectF(buttonRect), lightGray, gray);

        button->onLeftClick = [sharedData = sharedData, osc = osc](int x, int y) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)osc, 0 };
            sharedData->toAudio.enqueue(message);
        };

        container->pushChild(button);

        return container;
    }
};
