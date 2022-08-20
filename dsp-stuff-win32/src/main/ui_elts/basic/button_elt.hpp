#pragma once

#include "src/main/ui_elts/basic/base_elt.hpp"

class ButtonElt : public BaseElt {
public:
    bool outline = false;

    ButtonElt(GraphicsService* gfx_, D2D1_RECT_F rect_) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
    }

    void onDraw() override {
        gfx->drawRect(rect, black);
    }
};