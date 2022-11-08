#pragma once

#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"

class TextButtonElt : public BaseElt {
public:
    D2D1_COLOR_F passiveColor;
    D2D1_COLOR_F activeColor;
    bool isActive = false;
    std::wstring text;

    TextButtonElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        std::wstring _text,
        int x,
        int y,
        D2D1_COLOR_F _passiveColor=white,
        D2D1_COLOR_F _activeColor=blue,
        int _z=0,
        std::string _name = ""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        text = _text;

        int width = (int)(textWidth * text.length());
        setRects({ x, y, width, (int)textHeight });
        
        passiveColor = _passiveColor;
        activeColor = _activeColor;
        z = _z;
        name = _name;
    }

    void onDraw() override {
        gfx->outlineRect(rect, black, z + 1);

        gfx->drawText(text.c_str(), rect, black, z + 2);

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
