#pragma once

#include <iostream>
#include <string>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/util.hpp"

static std::vector<std::wstring> noteStrs = {
    L"C-", L"Db-", L"D-", L"Eb-", L"E-", L"F-", L"Gb-", L"G-", L"Ab-", L"A-", L"Bb-", L"B-",
    L"C0", L"Db0", L"D0", L"Eb0", L"E0", L"F0", L"Gb0", L"G0", L"Ab0", L"A0", L"Bb0", L"B0",
    L"C1", L"Db1", L"D1", L"Eb1", L"E1", L"F1", L"Gb1", L"G1", L"Ab1", L"A1", L"Bb1", L"B1",
    L"C2", L"Db2", L"D2", L"Eb2", L"E2", L"F2", L"Gb2", L"G2", L"Ab2", L"A2", L"Bb2", L"B2",
    L"C3", L"Db3", L"D3", L"Eb3", L"E3", L"F3", L"Gb3", L"G3", L"Ab3", L"A3", L"Bb3", L"B3",
    L"C4", L"Db4", L"D4", L"Eb4", L"E4", L"F4", L"Gb4", L"G4", L"Ab4", L"A4", L"Bb4", L"B4",
    L"C5", L"Db5", L"D5", L"Eb5", L"E5", L"F5", L"Gb5", L"G5", L"Ab5", L"A5", L"Bb5", L"B5",
    L"C6", L"Db6", L"D6", L"Eb6", L"E6", L"F6", L"Gb6", L"G6", L"Ab6", L"A6", L"Bb6", L"B6",
    L"C7", L"Db7", L"D7", L"Eb7", L"E7", L"F7", L"Gb7", L"G7", L"Ab7", L"A7", L"Bb7", L"B7",
};

class NoteNumberElt : public BaseElt {
public:
    BaseElt* container = nullptr;
    TextElt* text = nullptr;
    int number = 0;
    float f_number = 0.0f;
    float inc = 0.0f;
    std::function<void(int x)> setData;
    int min;
    int max;

    NoteNumberElt(
        GraphicsService* _gfx,
        int x,
        int y,
        std::function<void(int x)> _setData = [](int x) {},
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        number = 60;
        f_number = 60;
        min = 12;
        max = noteStrs.size();
        setData = _setData;

        RectWH rectWH = { x, y, (int)(3 * textWidth), (int)textHeight };
        
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
            noteStrs[number]
        );

        container->pushChild(text);

        onLeftClick = [this](int x, int y) {
            int offset = (rect.right - rect.left) - x;
            int incDigits = offset / textWidth;
            inc = pow(10, incDigits) / 10.0f;
        };

        onLeftDrag = [this](int x, int y, int xDelta, int yDelta) {
            if (yDelta > 0) {
                f_number = clamp(f_number - inc, (float)min, (float)max);
                number = f_number;
                setData(number);
                text->text = noteStrs[number];
            } else if (yDelta < 0) {
                f_number = clamp(f_number + inc, (float)min, (float)max - 1);
                number = f_number;
                setData(number);
                text->text = noteStrs[number];
            }
        };
    }

    void setNumber(int _number) {
        number = clamp(_number, min, max);
        text->text = noteStrs[number];
    }
};
