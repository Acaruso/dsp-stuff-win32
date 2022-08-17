#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class RectElt : public BaseElt {
public:
    bool outline = false;

    RectElt(GraphicsService* gfx, D2D1_RECT_F rect, bool outline=false) {
        this->gfx = gfx;
        this->rect = rect;
        this->relativeRect = rect;
        this->outline = outline;
    }

    void draw() override {
        if (outline) {
            gfx->outlineRect(rect, black);
        } else {
            gfx->drawRect(rect, black);
        }

        for (auto& child : children) {
            child->draw();
        }
    }
};
