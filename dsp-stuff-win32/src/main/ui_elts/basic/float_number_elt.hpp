#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
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
    int wholeDigits;
    int fracDigits;
    float number;
    float inc = 1.0f;
    int numWholeDigits;
    int numFracDigits;
    int numDigits;
    std::function<void(int x)> setData;
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
        std::function<void(int x)> _setData = [](int x) {},
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        min = _min;
        max = _max;
        numWholeDigits = _numWholeDigits;
        numFracDigits = _numFracDigits;
        numDigits = numWholeDigits + numFracDigits;
        wholeDigits = getWholeDigits(initialNumber);
        fracDigits = getFracDigits(initialNumber);
        setData = _setData;

        RectWH rectWH = {
            x,
            y,
            (int)(textWidth * (numDigits + 1)),   // need to account for '.' character
            (int)textHeight
        };

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
            makeString(wholeDigits, fracDigits)
        );

        container->pushChild(text);

        // onLeftClick = [this](int x, int y) {
        //     int offset = (rect.right - rect.left) - x;
        //     int incDigits = offset / textWidth;
        //     inc = pow(10, incDigits);
        // };

        onLeftClick = [](int x, int y) {
        };

        onLeftDrag = [this](int x, int y, int xDelta, int yDelta) {
            if (yDelta > 0) {
                number = clamp(number - inc, min, max);
                // setData(number);
                wholeDigits = getWholeDigits(number);
                fracDigits = getFracDigits(number);
                text->text = makeString(wholeDigits, fracDigits);
            } else if (yDelta < 0) {
                number = clamp(number + inc, min, max);
                // setData(number);
                wholeDigits = getWholeDigits(number);
                fracDigits = getFracDigits(number);
                text->text = makeString(wholeDigits, fracDigits);
            }
        };
    }

    int getWholeDigits(float f) {
        return (int)f;
    }

    int getFracDigits(float f) {
        return (int)((f - (int)f) * pow(10, numFracDigits));
    }

    float makeFloat(int wholeDigits, int fracDigits) {
        float f_whole = (float)wholeDigits;
        float f_frac = (float)fracDigits;

        return (f_whole + (f_frac / pow(10, numFracDigits)));
    }

    std::wstring makeString(int wholeDigits, int fracDigits) {
        std::wstringstream ss;
        ss << wholeDigits << "." << zeroExtendRight(fracDigits, numFracDigits);
        return alignRight(ss.str(), numDigits + 1);
    }

    void setNumber(float _number) {
        number = clamp(_number, min, max);
        wholeDigits = getWholeDigits(number);
        fracDigits = getFracDigits(number);
        text->text = makeString(wholeDigits, fracDigits);
    }
};
