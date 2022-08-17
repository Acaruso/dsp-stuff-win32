#pragma once

#include "src/main/constants.hpp"

class RectElt {
public:
    RectElt() {}

    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;

    void draw() {
        gfx->drawRect(rect, black);
    }
};
