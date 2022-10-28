#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/seqs/pattern_seq.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/toggle_button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class SeqGrid : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    BaseElt* container = nullptr;
    std::mutex* rootUgenLock;
    PatternSeq* patternSeq = nullptr;

    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    int numRows = 2;
    int numCols = 16;

    SeqGrid(
        GraphicsService* _gfx,
        InputState* _inputState,
        SharedData* _sharedData,
        PatternSeq* _patternSeq,
        int x,
        int y,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
        rootUgenLock = &sharedData->rootUgenLock;
        patternSeq = _patternSeq;

        RectWH rectWH = {
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding),
            (numRows * cellH) + ((numRows + 1) * padding)
        };

        rect = makeRectF(rectWH);
        absoluteRect = rect;

        z = _z;
        name = _name;

        // create ui elts /////////////////////////////////////////

        // create container
        container = new ContainerElt(gfx, makeRectF(0, 0, rectWH.w, rectWH.h), true);
        pushChild(container);

        // create grid rects
        int curX = padding;
        int curY = padding;

        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                ToggleButtonElt* button = new ToggleButtonElt(
                    gfx, 
                    inputState, 
                    makeRectF(curX, curY, cellW, cellH),
                    white,
                    gray,
                    green
                );

                button->isToggled = patternSeq->patterns[row][col].on;

                button->onLeftClick = [=](int x, int y) {
                    rootUgenLock->lock();
                    patternSeq->patterns[row][col].on = !patternSeq->patterns[row][col].on;
                    rootUgenLock->unlock();
                    button->isToggled = patternSeq->patterns[row][col].on;
                };

                container->pushChild(button);

                curX += cellW + padding;
            }

            curX = padding;
            curY += cellH + padding;
        }
    }
};
