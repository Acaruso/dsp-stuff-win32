#pragma once

#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"

class TextElt : public BaseElt {
public:
    std::wstring text;

    TextElt(GraphicsService* gfx, D2D1_RECT_F rect, std::wstring text) {
        this->gfx = gfx;
        this->rect = rect;
        this->relativeRect = rect;
        this->text = text;
    }

    void draw() override {
        gfx->drawText(text.c_str(), rect);

        for (auto& child : children) {
            child->draw();
        }
    }
};
