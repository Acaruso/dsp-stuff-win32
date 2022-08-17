#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"

class RectElt : public BaseElt {
public:
    RectElt(GraphicsService* gfx, D2D1_RECT_F rect) {
        this->gfx = gfx;
        this->rect = rect;
    }

    void draw() override {
        gfx->drawRect(rect, black);

        for (auto& child : children) {
            child->draw();
        }
    }
};
