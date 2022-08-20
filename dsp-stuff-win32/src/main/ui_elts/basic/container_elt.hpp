#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class ContainerElt : public BaseElt {
public:
    bool drawOutline = true;

    ContainerElt() {}

    ContainerElt(GraphicsService* gfx_, D2D1_RECT_F rect_) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
    }

    void onDraw() override {
        if (drawOutline) {
            gfx->outlineRect(rect, black);
        }
    }
};
