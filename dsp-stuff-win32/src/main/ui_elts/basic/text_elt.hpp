#pragma once

#include <iostream>
#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"

class TextElt : public BaseElt {
public:
    std::wstring text;
    D2D1_COLOR_F color;

    TextElt(
        GraphicsService* _gfx,
        RectWH _rectWH,
        std::wstring _text,
        D2D1_COLOR_F _color=black,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        setRects(_rectWH);
        text = _text;
        color = _color;
        z = _z;
        name = _name;
    }

    void onDraw() override {
        gfx->drawText(text.c_str(), rect, color, z);
    }
};
