#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/seqs/trigger_seq.hpp"
#include "src/audio/ugens/seqs/value_seq.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/toggle_button_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class ValueSeqGridElt : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    BaseElt* container = nullptr;
    GridElt* grid = nullptr;
    NumberElt* curNum = nullptr;
    NumberElt* defaultNum = nullptr;
    std::mutex* rootUgenLock;
    ValueSeq* seq = nullptr;

    RectWH rectWH;

    int numRows = 2;
    int numDisplayRows = 0;
    int numCols = 16;
    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    int selectedRow = -1;
    int selectedCol = -1;

    ValueSeqGridElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        SharedData* _sharedData,
        UiCompositeFactory* _uiCompositeFactory,
        ValueSeq* _seq,
        int x,
        int y,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
        uiCompositeFactory = _uiCompositeFactory;
        rootUgenLock = &sharedData->rootUgenLock;
        seq = _seq;

        numRows = seq->numTracks;

        numDisplayRows = numRows + 1;

        rectWH = {
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding) + 200,
            (numDisplayRows * cellH) + ((numDisplayRows + 1) * padding)
        };

        rect = makeRectF(rectWH);
        absoluteRect = rect;

        z = _z;
        name = _name;

        makeUiElts();
    }

    void makeUiElts() {
        container = new ContainerElt(gfx, makeRectF(0, 0, rectWH.w, rectWH.h), true);
        pushChild(container);

        grid = new GridElt(gfx, 0, 0, numDisplayRows, numCols, cellW, cellH, padding);
        container->pushChild(grid);

        makeNumberElts();
        makeTransport();
        makeSeqGrid();
    }

    void makeNumberElts() {
        // cur value
        RectWH r = makeRectWH(grid->rect);

        BaseElt* curNumContainer = uiCompositeFactory->makeNumberAndLabel(
            L"Cur Value",
            0,
            0,
            100000,
            r.w + padding,
            padding
        );

        container->pushChild(curNumContainer);

        curNum = (NumberElt*)curNumContainer->getElt("number");

        // default value
        RectWH r2 = makeRectWH(curNumContainer->rect);

        BaseElt* defaultNumContainer = uiCompositeFactory->makeNumberAndLabel(
            L"Default Value",
            0,
            0,
            100000,
            r2.x + r2.w + padding,
            padding
        );

        defaultNum = (NumberElt*)defaultNumContainer->getElt("number");

        container->pushChild(defaultNumContainer);
    }

    void makeTransport() {
        int row = 0;

        for (int col = 0; col < numCols; ++col) {
            ToggleButtonElt* elt = new ToggleButtonElt(
                gfx,
                inputState,
                makeRectF(0, 0, cellW, cellH),
                white,
                gray,
                green
            );

            grid->pushElt(row, col, elt);
        }
    }

    void makeSeqGrid() {
        for (int row = 1; row < numRows + 1; ++row) {
            for (int col = 0; col < numCols; ++col) {
                ToggleButtonElt* button = new ToggleButtonElt(
                    gfx,
                    inputState,
                    makeRectF(0, 0, cellW, cellH),
                    white,
                    gray,
                    green
                );

                button->isToggled = seq->patterns[row - 1][col].on;

                button->onLeftClick = [=](int x, int y) {
                    rootUgenLock->lock();

                    setSelected(row, col);

                    if (!getKeyState(VK_SHIFT)) {
                        ValueSeqCell& cell = seq->patterns[row - 1][col];

                        if (cell.on) {
                            cell.on = false;
                        } else {
                            cell.on = true;
                            cell.value = defaultNum->number;
                        }

                        button->isToggled = cell.on;
                    }

                    rootUgenLock->unlock();
                };

                grid->pushElt(row, col, button);
            }
        }
    }

    void setSelected(int row, int col) {
        if (selectedRow != -1 && selectedCol != -1) {
            ToggleButtonElt* prevElt = (ToggleButtonElt*)grid->getElt(selectedRow, selectedCol);
            prevElt->setIsSelected(false);
        }

        selectedRow = row;
        selectedCol = col;

        ToggleButtonElt* curElt = (ToggleButtonElt*)grid->getElt(selectedRow, selectedCol);
        curElt->setIsSelected(true);

        curNum->setNumber(seq->patterns[row - 1][col].value);
    }

    void onTick() override {
        if (!seq->on) {
            for (int i = 0; i < numCols; i++) {
                ToggleButtonElt* elt = (ToggleButtonElt*)grid->getElt(0, i);
                elt->isToggled = false;
            }
        } else {
            int count = seq->patternCounter;

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
