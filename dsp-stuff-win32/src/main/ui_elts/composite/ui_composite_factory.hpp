#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "src/audio/ugens/recorder.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/waveform_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/shared/shared_data.hpp"

class UiCompositeFactory {
private:
    GraphicsService* gfx;
    InputState* inputState;
    SharedData* sharedData;

public:
    UiCompositeFactory(GraphicsService* _gfx, InputState* _inputState, SharedData* _sharedData) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
    }

    BaseElt* makeButtonAndLabel(
        std::wstring labelStr,
        int x,
        int y,
        std::function<void(int, int)> onLeftClick
    ) {
        int buttonW = 40;
        int buttonH = 40;

        RectWH containerRect = { x, y, buttonW, (int)textHeight + buttonH };
        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect));

        RectWH labelRect = { 0, 0, containerRect.w, containerRect.h };
        TextElt* label = new TextElt(gfx, makeRectF(labelRect), labelStr);

        container->pushChild(label);

        RectWH buttonRect = { 0, (int)textHeight, 40, 40 };
        ButtonElt* button = new ButtonElt(
            gfx,
            inputState,
            makeRectF(buttonRect),
            lightGray,
            gray
        );

        button->onLeftClick = onLeftClick;

        container->pushChild(button);

        return container;
    }

    BaseElt* makeNumberAndLabel(
        std::wstring labelStr,
        int initialNumber,
        int min,
        int max,
        int x,
        int y,
        std::function<void(int x)> setData = [](int x) {}
    ) {
        int numDigits = getNumDigits(max - 1);
        int numberWidth = (int)(textWidth * numDigits);
        int labelWidth = (int)(textWidth * labelStr.length());
        int width = numberWidth > labelWidth ? numberWidth : labelWidth;

        RectWH containerRect = { x, y, width, (int)(textHeight * 2) };
        BaseElt* container = new ContainerElt(gfx, makeRectF(containerRect));

        RectWH labelRect = { 0, 0, containerRect.w, (int)(textHeight) };
        TextElt* label = new TextElt(gfx, makeRectF(labelRect), labelStr);

        container->pushChild(label);

        NumberElt* number = new NumberElt(
            gfx,
            initialNumber,
            min,
            max,
            0,
            textHeight,
            setData
        );

        container->pushChild(number);

        return container;
    }

    BaseElt* makeWaveContainer(SharedAudioBuffer* buffer, RectWH rect) {
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

    BaseElt* makeWaveAndButton(SharedAudioBuffer* buffer, RectWH rect) {
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

    BaseElt* makeTwoWavesAndButton(UgenManager* osc, BaseUgen* pBang, RectWH containerRect) {
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

        button->onLeftClick = [sharedData = sharedData, pBang = pBang](int x, int y) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            sharedData->toAudio.enqueue(message);
        };

        container->pushChild(button);

        return container;
    }
};
