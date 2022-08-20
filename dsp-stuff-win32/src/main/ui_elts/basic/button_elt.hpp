#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class ButtonElt : public BaseElt {
public:
    D2D1_COLOR_F color;
    D2D1_COLOR_F passiveColor;
    D2D1_COLOR_F activeColor;

    ButtonElt(
        GraphicsService* gfx_,
        InputState* inputState_,
        D2D1_RECT_F rect_,
        D2D1_COLOR_F color_=white,
        D2D1_COLOR_F activeColor_=black
    ) {
        gfx = gfx_;
        inputState = inputState_;
        rect = rect_;
        absoluteRect = rect_;
        color = color_;
        passiveColor = color_;
        activeColor = activeColor_;
    }

    void onDraw() override {
        gfx->outlineRect(rect, black);
        gfx->drawRect(rect, color);
    }

    void onTick() override {
        if (isInsideRect(inputState->mouseX, inputState->mouseY, absoluteRect)) {
        }
    }
};