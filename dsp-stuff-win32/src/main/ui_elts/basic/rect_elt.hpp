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
        if (outline) {
            D2D1_POINT_2F topLeft = D2D1::Point2F(rect.left, rect.top);
            D2D1_POINT_2F topRight = D2D1::Point2F(rect.right, rect.top);
            D2D1_POINT_2F bottomRight = D2D1::Point2F(rect.right, rect.bottom);
            D2D1_POINT_2F bottomLeft = D2D1::Point2F(rect.left, rect.bottom);

            gfx->drawLine(topLeft, topRight, color, z);
            gfx->drawLine(topRight, bottomRight, color, z);
            gfx->drawLine(bottomRight, bottomLeft, color, z);
            gfx->drawLine(bottomLeft, topLeft, color, z);
        } else {
            gfx->drawRect(rect, color, z);
        }
    }
};
