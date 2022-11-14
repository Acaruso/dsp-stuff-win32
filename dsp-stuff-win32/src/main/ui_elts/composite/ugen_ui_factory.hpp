#pragma once

#include "src/audio/ugens/bang.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class UgenUiFactory {
    GraphicsService* gfx;
    SharedData* sharedData;
    InputState* inputState;

public:
    UgenUiFactory(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
    }

    BaseElt* makeBangButton(BaseUgen* pBang, int x, int y) {
        ButtonElt* button = new ButtonElt(
            gfx, 
            inputState, 
            {20, 20, 50, 50}, 
            lightGray, 
            gray
        );

        SharedData* pSharedData = sharedData;

        button->onLeftClick = [pSharedData = pSharedData, pBang = pBang](
            int x, 
            int y
        ) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            pSharedData->toAudio.enqueue(message);
        };

        return button;
    }
};