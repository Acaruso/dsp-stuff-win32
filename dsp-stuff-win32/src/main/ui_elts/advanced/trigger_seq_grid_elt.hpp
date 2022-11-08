#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/seqs/trigger_seq.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/toggle_button_elt.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class TriggerSeqGridElt : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    BaseElt* container = nullptr;
    GridElt* grid = nullptr;
    std::mutex* rootUgenLock;
    TriggerSeq* triggerSeq = nullptr;

    int numRows = 2;
    int numDisplayRows = 0;
    int numCols = 16;
    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    TriggerSeqGridElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        SharedData* _sharedData,
        TriggerSeq* _triggerSeq,
        int x,
        int y,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
        rootUgenLock = &sharedData->rootUgenLock;
        triggerSeq = _triggerSeq;

        numDisplayRows = numRows + 1;

        setRects({
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding),
            (numDisplayRows * cellH) + ((numDisplayRows + 1) * padding)
        });

        z = _z;
        name = _name;

        makeUiElts();
    }

    void makeUiElts() {
        container = new ContainerElt(gfx, { 0, 0, rectWH.w, rectWH.h }, true);
        pushChild(container);

        grid = new GridElt(gfx, 0, 0, numDisplayRows, numCols, cellW, cellH, padding);
        container->pushChild(grid);

        makeTransport();
        makeSeqGrid();
    }

    void makeTransport() {
        int row = 0;

        for (int col = 0; col < numCols; ++col) {
            ToggleButtonElt* elt = new ToggleButtonElt(
                gfx,
                inputState,
                { 0, 0, cellW, cellH },
                white,
                gray,
                green
            );

            grid->pushElt(row, col, elt);
        }
    }

    void makeSeqGrid() {
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                ToggleButtonElt* button = new ToggleButtonElt(
                    gfx,
                    inputState,
                    { 0, 0, cellW, cellH },
                    white,
                    gray,
                    green
                );

                button->isToggled = triggerSeq->patterns[row][col].on;

                button->onLeftClick = [=](int x, int y) {
                    rootUgenLock->lock();
                    triggerSeq->patterns[row][col].on = !triggerSeq->patterns[row][col].on;
                    rootUgenLock->unlock();
                    button->isToggled = triggerSeq->patterns[row][col].on;
                };

                grid->pushElt(row + 1, col, button);
            }
        }
    }

    void onTick() override {
        if (!triggerSeq->on) {
            for (int i = 0; i < numCols; i++) {
                ToggleButtonElt* elt = (ToggleButtonElt*)grid->getElt(0, i);
                elt->isToggled = false;
            }
        } else {
            int count = triggerSeq->patternCounter;

            ToggleButtonElt* elt = (ToggleButtonElt*)grid->getElt(0, count);
            elt->isToggled = true;

            ToggleButtonElt* prevElt = (ToggleButtonElt*)grid->getElt(
                0,
                modDec(count, numCols)
            );
            prevElt->isToggled = false;
        }
    }
};
