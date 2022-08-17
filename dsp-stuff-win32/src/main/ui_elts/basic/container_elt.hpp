#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"

class ContainerElt : public BaseElt {
public:
    bool drawOutline = true;

    ContainerElt() {}

    ContainerElt(GraphicsService* gfx, D2D1_RECT_F rect) {
        this->gfx = gfx;
        this->rect = rect;
        this->relativeRect = rect;
    }

    void draw() override {
        if (drawOutline) {
            gfx->outlineRect(rect, black);
        }

        gfx->pushOffset(rect);

        for (auto& child : children) {
            child->draw();
        }

        gfx->popOffset();
    }
};
