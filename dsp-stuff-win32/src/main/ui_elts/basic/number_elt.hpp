#pragma once

#include <iostream>
#include <string>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/util.hpp"

class NumberElt : public BaseElt {
public:
    BaseElt* container = nullptr;
    TextElt* text = nullptr;
    int number = 0;
    int inc;
    int numDigits;
    std::function<void(int x)> setData;
    int min;
    int max;

    NumberElt(
        GraphicsService* _gfx,
        int initialNumber,
        int _min,
        int _max,
        int x,
        int y,
        std::function<void(int x)> _setData = [](int x) {},
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        number = initialNumber;
        min = _min;
        max = _max;
        setData = _setData;

        numDigits = getNumDigits(max - 1);
        RectWH rectWH = { x, y, (int)(textWidth * numDigits), (int)textHeight };
        
        rect = makeRectF(rectWH);
        absoluteRect = rect;

        z = _z;
        name = _name;

        container = new ContainerElt(gfx, makeRectF(0, 0, rectWH.w, rectWH.h), true);
        pushChild(container);

        RectWH textRect = { 0, 0, rectWH.w, rectWH.h };
        text = new TextElt(
            gfx,
            makeRectF(textRect),
            alignRight(number, getNumDigits(max - 1))
        );

        container->pushChild(text);

        onLeftClick = [this](int x, int y) {
            int offset = (rect.right - rect.left) - x;
            int incDigits = offset / textWidth;
            inc = pow(10, incDigits);
        };

        onLeftDrag = [this](int x, int y, int xDelta, int yDelta) {
            if (yDelta > 0) {
                number = clamp(number - inc, min, max);
                setData(number);
                text->text = alignRight(number, numDigits);
            } else if (yDelta < 0) {
                number = clamp(number + inc, min, max);
                setData(number);
                text->text = alignRight(number, numDigits);
            }
        };
    }
};
