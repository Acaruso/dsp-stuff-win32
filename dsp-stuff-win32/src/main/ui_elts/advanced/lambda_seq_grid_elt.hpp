#pragma once

#include <string>
#include <vector>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/seqs/lambda_seq.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/float_number_elt.hpp"
#include "src/main/ui_elts/basic/note_number_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/text_button_elt.hpp"
#include "src/main/ui_elts/basic/toggle_button_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class LambdaSeqGridElt : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    ContainerElt* container = nullptr;
    GridElt* grid = nullptr;
    NumberElt* curValueInt = nullptr;
    NumberElt* defaultValueInt = nullptr;
    NoteNumberElt* curValueNote = nullptr;
    NoteNumberElt* defaultValueNote = nullptr;
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
                    t->isToggled = false;
                    curValueInt->setNumber(c->value);
                } else if (keyCode == int('C')) {
                    copiedCell = *getSelectedSeqCell();
                } else if (keyCode == int('V')) {
                    LambdaSeqCell* c = getSelectedSeqCell();
                    *c = copiedCell;
                    ToggleButtonElt* t = getSelectedButton();
                    t->isToggled = c->on;
                    curValueInt->setNumber(c->value);
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

    void setSelectedSeqCellValue(float newValue) {
        LambdaSeqCell* cell = getSelectedSeqCell();
        if (cell != nullptr) {
            cell->value = newValue;
        }
    }

    void makeUiElts() {
        container = (ContainerElt*)pushChild(
            new ContainerElt(gfx, makeRectF(0, 0, rectWH.w, rectWH.h), true)
        );

        grid = (GridElt*)container->pushChild(
            new GridElt(gfx, 0, 0, numDisplayRows, numCols, cellW, cellH, padding)
        );

        makeTransport();
        makeSeqGrid();
        makeNumberElts();
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
                                cell->value = defaultValueInt->number;
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

        curValueInt->setNumber(getSelectedSeqCellValue());
    }

    void makeNumberElts() {
        // cur value int
        RectWH gridRect = makeRectWH(grid->rect);

        BaseElt* curValueIntContainer = uiCompositeFactory->makeNumberAndLabel(
            L"Cur Value",
            0,
            0,
            100000,
            gridRect.w + padding,
            padding,
            [=](int newNumber) {
                setSelectedSeqCellValue(newNumber);
            }
        );

        curValueInt = (NumberElt*)curValueIntContainer->getElt("number");

        container->pushChild(curValueIntContainer);

        // default value int
        RectWH curValueIntRect = makeRectWH(curValueIntContainer->rect);

        BaseElt* defaultValueIntContainer = uiCompositeFactory->makeNumberAndLabel(
            L"Default Value",
            1,
            0,
            100000,
            curValueIntRect.x + curValueIntRect.w + padding,
            padding
        );

        defaultValueInt = (NumberElt*)defaultValueIntContainer->getElt("number");

        container->pushChild(defaultValueIntContainer);

        // cur value note
        BaseElt* curValueNoteContainer = uiCompositeFactory->makeNoteNumberAndLabel(
            L"Cur Value",
            60,
            gridRect.w + padding,
            padding,
            [=](float newNumber) {
                setSelectedSeqCellValue(newNumber);
            }
        );

        curValueNoteContainer->visible = false;

        curValueNote = (NoteNumberElt*)curValueNoteContainer->getElt("number");

        container->pushChild(curValueNoteContainer);

        // default value note
        RectWH curValueNoteRect = makeRectWH(curValueNoteContainer->rect);

        BaseElt* defaultValueNoteContainer = uiCompositeFactory->makeNoteNumberAndLabel(
            L"Default Value",
            60,
            curValueNoteRect.x + curValueNoteRect.w + padding,
            padding
        );
        defaultValueNoteContainer->visible = false;

        defaultValueNote = (NoteNumberElt*)defaultValueNoteContainer->getElt("number");

        container->pushChild(defaultValueNoteContainer);

        // int button
        TextButtonElt* intButton = new TextButtonElt(
            gfx,
            inputState,
            L"Int",
            curValueIntRect.x,
            curValueIntRect.y + 50
        );

        intButton->onLeftClick = [=](int x, int y) {
            curValueIntContainer->visible      = true;
            defaultValueIntContainer->visible  = true;
            curValueNoteContainer->visible     = false;
            defaultValueNoteContainer->visible = false;
        };

        container->pushChild(intButton);

        // note button
        RectWH intButtonRect = makeRectWH(intButton->rect);

        TextButtonElt* noteButton = new TextButtonElt(
            gfx,
            inputState,
            L"Note",
            intButtonRect.x + intButtonRect.w + 10,
            intButtonRect.y
        );

        noteButton->onLeftClick = [=](int x, int y) {
            curValueIntContainer->visible      = false;
            defaultValueIntContainer->visible  = false;
            curValueNoteContainer->visible     = true;
            defaultValueNoteContainer->visible = true;
        };

        container->pushChild(noteButton);
    }

    void onTick() override {
        if (!seq->on) {
            for (int i = 0; i < numCols; i++) {
                ToggleButtonElt* elt = (ToggleButtonElt*)grid->getElt(0, i);
                elt->isToggled = false;
            }
        } else {
            ToggleButtonElt* elt = (ToggleButtonElt*)grid->getElt(0, seq->stepIdx);
            elt->isToggled = true;

            ToggleButtonElt* prevElt = (ToggleButtonElt*)grid->getElt(
                0,
                modDec(seq->stepIdx, numCols)
            );
            prevElt->isToggled = false;
        }
    }
};
