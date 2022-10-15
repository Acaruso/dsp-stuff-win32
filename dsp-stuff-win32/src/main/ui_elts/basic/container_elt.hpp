#pragma once

#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class ContainerElt : public BaseElt {
public:
    bool outline = true;

    ContainerElt() {}

    ContainerElt(
        GraphicsService* _gfx,
        D2D1_RECT_F _rect,
        bool _outline=false,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        rect = _rect;
        absoluteRect = _rect;
        outline = _outline;
        z = _z;
        name = _name;
    }

    void onDraw() override {
        if (outline) {
            gfx->outlineRect(rect, black, z);
        }
    }
};
