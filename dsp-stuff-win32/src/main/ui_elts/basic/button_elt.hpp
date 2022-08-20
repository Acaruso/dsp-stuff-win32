#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class ButtonElt : public BaseElt {
public:
    D2D1_COLOR_F color;

    ButtonElt(GraphicsService* gfx_, D2D1_RECT_F rect_, D2D1_COLOR_F color_=black) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
        color = color_;
    }

    void onDraw() override {
        gfx->outlineRect(rect, black);
        gfx->drawRect(rect, color);
    }
};