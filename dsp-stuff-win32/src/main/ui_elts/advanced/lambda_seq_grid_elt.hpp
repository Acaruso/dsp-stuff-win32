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
    FloatNumberElt* curValueFloat = nullptr;
    FloatNumberElt* defaultValueFloat = nullptr;
    NoteNumberElt* curValueNote = nullptr;
    NoteNumberElt* defaultValueNote = nullptr;
    std::mutex* rootUgenLock;
    LambdaSeq* seq = nullptr;
    LambdaSeqCell copiedCell;

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

        setRects({
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding) + 200,
            (numDisplayRows * cellH) + ((numDisplayRows + 1) * padding)
        });

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
                    curValueFloat->setNumber(c->value);
                } else if (keyCode == int('C')) {
                    copiedCell = *getSelectedSeqCell();
                } else if (keyCode == int('V')) {
                    LambdaSeqCell* c = getSelectedSeqCell();
                    *c = copiedCell;
                    ToggleButtonElt* t = getSelectedButton();
                    t->isToggled = c->on;
                    curValueFloat->setNumber(c->value);
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

    float getSelectedSeqCellValue() {
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
            new ContainerElt(gfx, { 0, 0, rectWH.w, rectWH.h }, true)
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
                { 0, 0, cellW, cellH },
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
                    { 0, 0, cellW, cellH },
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
                                cell->value = defaultValueFloat->number;
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

        curValueFloat->setNumber(getSelectedSeqCellValue());
    }

    void makeNumberElts() {
        // cur value int
        BaseElt* curValueFloatContainer = uiCompositeFactory->makeFloatNumberAndLabel(
            L"Cur Value",
            0.0f,
            0.0f,
            20000.0f,
            5,
            2,
            grid->rectWH.w + padding,
            padding,
            [=](int newNumber) {
                setSelectedSeqCellValue(newNumber);
            }
        );

        curValueFloat = (FloatNumberElt*)curValueFloatContainer->getElt("number");

        container->pushChild(curValueFloatContainer);

        // default value int
        BaseElt* defaultValueFloatContainer = uiCompositeFactory->makeFloatNumberAndLabel(
            L"Default Value",
            1.0f,
            0.0f,
            20000.0f,
            5,
            2,
            curValueFloatContainer->rectWH.x + curValueFloatContainer->rectWH.w + padding,
            padding
        );

        defaultValueFloat = (FloatNumberElt*)defaultValueFloatContainer->getElt("number");

        container->pushChild(defaultValueFloatContainer);

        // cur value note
        curValueNote = new NoteNumberElt(
            gfx,
            60,
            grid->rectWH.w + padding,
            50,
            [=](float newNumber) {
                setSelectedSeqCellValue(newNumber);
                curValueFloat->setNumber(newNumber);
            }
        );

        container->pushChild(curValueNote);

        // default value note
        defaultValueNote = new NoteNumberElt(
            gfx,
            60,
            curValueFloatContainer->rectWH.x + curValueFloatContainer->rectWH.w + padding,
            50,
            [=](float newNumber) {
                defaultValueFloat->setNumber(newNumber);
            }
        );

        container->pushChild(defaultValueNote);
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
