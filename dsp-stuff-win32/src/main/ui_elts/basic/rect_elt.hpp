#pragma once

#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class RectElt : public BaseElt {
public:
    D2D1_COLOR_F color = black;
    bool outline = false;
    bool show = true;

    RectElt(
        GraphicsService* _gfx,
        D2D1_RECT_F _rect,
        D2D1_COLOR_F _color=black,
        bool _outline=false,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        rect = _rect;
        absoluteRect = _rect;
        color = _color;
        outline = _outline;
        z = _z;
        name = _name;
    }

    void onDraw() override {
        if (!show) {
            return;
        } else if (outline) {
            gfx->outlineRect(rect, color, z);
        } else {
            gfx->drawRect(rect, color, z);
        }
    }
};
