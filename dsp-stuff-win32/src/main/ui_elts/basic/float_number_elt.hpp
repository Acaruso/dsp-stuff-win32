#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/util.hpp"

class FloatNumberElt : public BaseElt {
public:
    BaseElt* container = nullptr;
    TextElt* text = nullptr;
    float number;
    float inc = 1.0f;
    int numWholeDigits;
    int numFracDigits;
    int numDigits;
    std::function<void(float x)> setData;
    float min;
    float max;

    FloatNumberElt(
        GraphicsService* _gfx,
        float initialNumber,
        float _min,
        float _max,
        int _numWholeDigits,
        int _numFracDigits,
        int x,
        int y,
        std::function<void(float x)> _setData = [](int x) {},
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        number = initialNumber;
        min = _min;
        max = _max;
        numWholeDigits = _numWholeDigits;
        numFracDigits = _numFracDigits;
        numDigits = numWholeDigits + numFracDigits + 1;  // need to account for '.'
        setData = _setData;

        rect = {
            x,
            y,
            (int)(textWidth * numDigits),
            (int)textHeight
        };

        setRects({ x, y, (int)(textWidth * numDigits), (int)textHeight });

        z = _z;
        name = _name;

        container = new ContainerElt(gfx, { 0, 0, rect.w, rect.h }, true);
        pushChild(container);

        text = new TextElt(
            gfx,
            { 0, 0, rect.w, rect.h },
            makeString(number)
        );

        container->pushChild(text);

        onLeftClick = [this](int x, int y) {
            int offset = (relRect.right - relRect.left) - x;
            int incDigits = offset / textWidth;

            if (incDigits < numFracDigits) {
                inc = 1.0f / pow(10, (numFracDigits - incDigits));
            } else if (incDigits == numFracDigits) {
                inc = 1.0f;
            } else if (incDigits > numFracDigits) {
                inc = pow(10, (incDigits - (numFracDigits + 1)));
            }
        };

        onLeftDrag = [this](int x, int y, int xDelta, int yDelta) {
            if (yDelta > 0) {
                setNumber(number - inc);
                setData(number);
            } else if (yDelta < 0) {
                setNumber(number + inc);
                setData(number);
            }
        };
    }

    std::wstring makeString(float number) {
        std::wstringstream ss;
        ss.precision(2);
        ss << std::fixed << _round(number, numFracDigits);
        return alignRight(ss.str(), numDigits);
    }

    void setNumber(float newNumber) {
        number = _round(
            clamp(newNumber, min, max),
            numFracDigits
        );
        text->text = makeString(number);
    }
};
