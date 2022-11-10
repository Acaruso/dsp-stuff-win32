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
#include "src/main/ui_elts/basic/float_number_elt.hpp"
#include "src/main/ui_elts/basic/note_number_elt.hpp"
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

        BaseElt* container = new ContainerElt(
            gfx,
            { x, y, buttonW, (int)textHeight + buttonH }
        );

        container->pushChild(
            new TextElt(
                gfx,
                { 0, 0, container->rect.w, container->rect.h },
                labelStr
            )
        );

        ButtonElt* button = new ButtonElt(
            gfx,
            inputState,
            { 0, (int)textHeight, 40, 40 },
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

        BaseElt* container = new ContainerElt(
            gfx,
            { x, y, width, (int)(textHeight * 2) }
        );

        container->pushChild(
            new TextElt(
                gfx,
                { 0, 0, container->rect.w, (int)(textHeight) },
                labelStr
            )
        );

        NumberElt* number = new NumberElt(
            gfx,
            initialNumber,
            min,
            max,
            0,
            textHeight,
            setData
        );
        number->name = "number";
        container->pushChild(number);

        return container;
    }

    BaseElt* makeNoteNumberAndLabel(
        std::wstring labelStr,
        int initialNumber,
        int x,
        int y,
        std::function<void(float x)> setData = [](int x) {}
    ) {
        int numberWidth = (int)(textWidth * 3);
        int labelWidth = (int)(textWidth * labelStr.length());
        int width = numberWidth > labelWidth ? numberWidth : labelWidth;

        BaseElt* container = new ContainerElt(gfx, { x, y, width, (int)(textHeight * 2) });

        container->pushChild(
            new TextElt(
                gfx,
                { 0, 0, container->rect.w, (int)(textHeight) },
                labelStr
            )
        );

        NoteNumberElt* number = new NoteNumberElt(
            gfx,
            initialNumber,
            0,
            textHeight,
            setData
        );
        number->name = "number";
        container->pushChild(number);

        return container;
    }

    BaseElt* makeFloatNumberAndLabel(
        std::wstring labelStr,
        float initialNumber,
        float min,
        float max,
        int numWholeDigits,
        int numFracDigits,
        int x,
        int y,
        std::function<void(float x)> setData = [](int x) {}
    ) {
        int numberWidth = (int)(textWidth * (numWholeDigits + numFracDigits + 1));
        int labelWidth = (int)(textWidth * labelStr.length());
        int width = numberWidth > labelWidth ? numberWidth : labelWidth;

        BaseElt* container = new ContainerElt(gfx, { x, y, width, (int)(textHeight * 2) });

        container->pushChild(
            new TextElt(
                gfx,
                { 0, 0, container->rect.w, (int)(textHeight) },
                labelStr
            )
        );

        FloatNumberElt* number = new FloatNumberElt(
            gfx,
            initialNumber,
            min,
            max,
            numWholeDigits,
            numFracDigits,
            0,
            textHeight,
            setData
        );
        number->name = "number";
        container->pushChild(number);

        return container;
    }

    BaseElt* makeWaveContainer(SharedAudioBuffer* buffer, RectWH rect) {
        BaseElt* container = new ContainerElt(gfx, rect, true);

        container->pushChild(
            new WaveformElt(
                gfx,
                buffer,
                inputState,
                sharedData,
                { 0, 0, container->rect.w, container->rect.h }
            )
        );

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

        BaseElt* container = new ContainerElt(gfx, containerRect, true);

        container->pushChild(new RectElt(gfx, { 0, 0, rect.w, rect.h }, blue, false, -1));

        BaseElt* wave = makeWaveContainer(buffer, waveRect);
        container->pushChild(wave);

        ButtonElt* button = new ButtonElt(gfx, inputState, buttonRect, lightGray, gray);
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
        BaseElt* container = new ContainerElt(gfx, containerRect, true);

        // outer container background
        container->pushChild(new RectElt(gfx, { 0, 0, containerRect.w, containerRect.h }, blue, false, -1));

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

        ButtonElt* button = new ButtonElt(gfx, inputState, buttonRect, lightGray, gray);

        button->onLeftClick = [sharedData = sharedData, pBang = pBang](int x, int y) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            sharedData->toAudio.enqueue(message);
        };

        container->pushChild(button);

        return container;
    }
};
