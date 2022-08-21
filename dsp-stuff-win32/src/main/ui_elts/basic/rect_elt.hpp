#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class RectElt : public BaseElt {
public:
    bool outline = false;

    RectElt(GraphicsService* gfx_, D2D1_RECT_F rect_, bool outline_=false, int z_=0) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
        outline = outline_;
        z = z_;
    }

    void onDraw() override {
        if (outline) {
            gfx->outlineRect(rect, black, z);
        } else {
            gfx->drawRect(rect, black, z);
        }
    }
};
