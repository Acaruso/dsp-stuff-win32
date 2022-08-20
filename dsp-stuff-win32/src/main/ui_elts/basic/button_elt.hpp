#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"

class ButtonElt : public BaseElt {
public:
    D2D1_COLOR_F color;
    D2D1_COLOR_F passiveColor;
    D2D1_COLOR_F activeColor;
    bool isActive = false;

    ButtonElt(
        GraphicsService* gfx_,
        InputState* inputState_,
        D2D1_RECT_F rect_,
        D2D1_COLOR_F passiveColor_=white,
        D2D1_COLOR_F activeColor_=black
    ) {
        gfx = gfx_;
        inputState = inputState_;
        rect = rect_;
        absoluteRect = rect_;
        passiveColor = passiveColor_;
        activeColor = activeColor_;
    }

    void onDraw() override {
        gfx->outlineRect(rect, black);

        if (isActive) {
            gfx->drawRect(rect, activeColor);
        } else {
            gfx->drawRect(rect, passiveColor);
        }
    }

    void onTick() override {
        if (
            inputState->isActiveWindow 
            && getKeyState(VK_LBUTTON) 
            && isInsideRect(inputState->mouseX, inputState->mouseY, absoluteRect)
        ) {
            isActive = true;
        } else {
            isActive = false;
        }
    }
};
