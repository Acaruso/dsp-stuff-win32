#pragma once

#include <string>

#include "src/main/constants.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"

class ToggleButtonElt : public BaseElt {
public:
    D2D1_COLOR_F color;
    D2D1_COLOR_F passiveColor;
    D2D1_COLOR_F activeColor;
    bool isActive = false;

    ToggleButtonElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        D2D1_RECT_F _rect,
        D2D1_COLOR_F _passiveColor=white,
        D2D1_COLOR_F _activeColor=black,
        int _z=0,
        std::string _name = ""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        rect = _rect;
        absoluteRect = _rect;
        passiveColor = _passiveColor;
        activeColor = _activeColor;
        z = _z;
        name = _name;
    }

    void onDraw() override {
        gfx->outlineRect(rect, black, z);

        if (isActive) {
            gfx->drawRect(rect, activeColor, z);
        } else {
            gfx->drawRect(rect, passiveColor, z);
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
