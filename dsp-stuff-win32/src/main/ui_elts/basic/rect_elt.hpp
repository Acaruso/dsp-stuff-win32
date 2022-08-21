#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class RectElt : public BaseElt {
public:
    D2D1_COLOR_F color = black;
    bool outline = false;

    RectElt(
        GraphicsService* gfx_, 
        D2D1_RECT_F rect_, 
        D2D1_COLOR_F color_=black,
        bool outline_=false, 
        int z_=0
    ) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
        color = color_;
        outline = outline_;
        z = z_;
    }

    void onDraw() override {
        if (outline) {
            gfx->outlineRect(rect, color, z);
        } else {
            gfx->drawRect(rect, color, z);
        }
    }
};
