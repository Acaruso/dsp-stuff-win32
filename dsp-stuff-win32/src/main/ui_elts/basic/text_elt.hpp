#pragma once

#include <iostream>
#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"

class TextElt : public BaseElt {
public:
    std::wstring text;

    TextElt(GraphicsService* gfx_, D2D1_RECT_F rect_, std::wstring text_) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
        text = text_;

        onDraw = [&]() {
            gfx->drawText(text.c_str(), rect);
        };

        onLeftClick = [&](int x, int y) {
            std::string s = rectToString(this->rect);
            std::cout << s << std::endl;
        };
    }
};
