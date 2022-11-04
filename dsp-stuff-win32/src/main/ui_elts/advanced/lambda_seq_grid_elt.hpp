#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/seqs/lambda_seq.hpp"
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

class LambdaSeqGridElt : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    BaseElt* container = nullptr;
    GridElt* grid = nullptr;
    NumberElt* curNum = nullptr;
    NumberElt* defaultNum = nullptr;
    std::mutex* rootUgenLock;
    LambdaSeq* seq = nullptr;
    LambdaSeqCell copiedCell;

    RectWH rectWH;

    int numRows = 2;
    int numDisplayRows = 0;
    int numCols = 16;
    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    int selectedRow = -1;
    int selectedCol = -1;

    LambdaSeqGridElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        SharedData* _sharedData,
        UiCompositeFactory* _uiCompositeFactory,
        LambdaSeq* _seq,
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

        onKeyDown = [&](int keyCode) {
            if (getKeyState(VK_CONTROL)) {
                if (keyCode == int('X')) {
                    LambdaSeqCell* c = getSelectedSeqCell();
                    copiedCell = *c;
                    *c = LambdaSeqCell{};

                    ToggleButtonElt* t = getSelectedButton();
                    t->isToggled = c->on;
                    curNum->setNumber(c->value);
                } else if (keyCode == int('C')) {
                    copiedCell = *getSelectedSeqCell();
                } else if (keyCode == int('V')) {
                    LambdaSeqCell* c = getSelectedSeqCell();
                    *c = copiedCell;

                    ToggleButtonElt* t = getSelectedButton();
                    t->isToggled = c->on;
                    curNum->setNumber(c->value);
                }
            }
        };

        makeUiElts();
    }

    ToggleButtonElt* getSelectedButton() {
        return (ToggleButtonElt*)grid->getElt(selectedRow, selectedCol);
    }

    LambdaSeqCell* getSeqCell(int row, int col) {
        return &seq->patterns[row - 1][col];
    }

    LambdaSeqCell* getSelectedSeqCell() {
        if (selectedRow != -1 && selectedCol != -1) {
            return &seq->patterns[selectedRow - 1][selectedCol];
        } else {
            return nullptr;
        }
    }

    int getSelectedSeqCellValue() {
        LambdaSeqCell* cell = getSelectedSeqCell();
        if (cell != nullptr) {
            return cell->value;
        } else {
            return 0;
        }
    }

    void setSelectedSeqCellValue(int newValue) {
        LambdaSeqCell* cell = getSelectedSeqCell();
        if (cell != nullptr) {
            cell->value = newValue;
        }
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

        curNum = (NumberElt*)curNumContainer->getElt("number");

        curNum->setData = [=](int newNumber) {
            setSelectedSeqCellValue(newNumber);
        };

        container->pushChild(curNumContainer);

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

                    if (!getKeyState(VK_SHIFT)) {
                        LambdaSeqCell* cell = getSeqCell(row, col);

                        if (cell->on) {
                            if (getKeyState(VK_CONTROL)) {
                                cell->value = 1.0f;
                            } else {
                                cell->on = false;
                            }
                        } else {
                            cell->on = true;
                            if (getKeyState(VK_CONTROL)) {
                                cell->value = 1.0f;
                            } else {
                                cell->value = defaultNum->number;
                            }
                        }

                        button->isToggled = cell->on;
                    }

                    setSelected(row, col);

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

        curNum->setNumber(getSelectedSeqCellValue());
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
