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

    TextElt(GraphicsService* gfx_, D2D1_RECT_F rect_, std::wstring text_, D2D1_COLOR_F color_=black, int z_=0) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
        text = text_;
        color = color_;
        z = z_;
    }

    void onDraw() override {
        gfx->drawText(text.c_str(), rect, color, z);
    }
};
