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
    D2D1_COLOR_F toggledColor;
    bool isActive = false;
    bool isToggled = false;
    bool isSelected = false;

    ToggleButtonElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        RectWH _rectWH,
        D2D1_COLOR_F _passiveColor=white,
        D2D1_COLOR_F _activeColor=black,
        D2D1_COLOR_F _toggledColor=green,
        int _z=0,
        std::string _name = ""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        setRects(_rectWH);
        passiveColor = _passiveColor;
        activeColor = _activeColor;
        toggledColor = _toggledColor;
        z = _z;
        name = _name;
    }

    void toggle() {
        isToggled = !isToggled;
    }

    void setIsSelected(bool _isSelected) {
        isSelected = _isSelected;
    }

    void toggleIsSelected() {
        isSelected = !isSelected;
    }

    void onDraw() override {
        gfx->outlineRect(relRect, black, z + 1);

        if (isActive) {
            gfx->drawRect(relRect, activeColor, z);
        } else if (isToggled) {
            gfx->drawRect(relRect, toggledColor, z);
        } else {
            gfx->drawRect(relRect, passiveColor, z);
        }

        if (isSelected) {
            gfx->outlineRect(
                makeRectF(
                    rect.x + 2,
                    rect.y + 2,
                    rect.w - 4,
                    rect.h - 4
                ), 
                black,
                z + 1
            );
        }
    }

    void onTick() override {
        if (
            inputState->isActiveWindow
            && getKeyState(VK_LBUTTON)
            && isInsideRect(inputState->mouseX, inputState->mouseY, absRect)
        ) {
            isActive = true;
        } else {
            isActive = false;
        }
    }
};
