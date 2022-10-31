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

// class ValueSeqGridElt : public BaseElt {
// public:
//     SharedData* sharedData = nullptr;
//     BaseElt* container = nullptr;
//     GridElt* grid = nullptr;
//     std::mutex* rootUgenLock;

//     RectWH rectWH;

//     int numRows = 2;
//     int numDisplayRows = 0;
//     int numCols = 5;
//     int cellW = 30;
//     int cellH = 30;
//     int padding = 5;

//     int selectedRow = -1;
//     int selectedCol = -1;

//     ValueSeqGridElt(
//         GraphicsService* _gfx,
//         InputState* _inputState,
//         SharedData* _sharedData,
//         int x,
//         int y,
//         int _z=0,
//         std::string _name=""
//     ) {
//         gfx = _gfx;
//         inputState = _inputState;
//         sharedData = _sharedData;
//         rootUgenLock = &sharedData->rootUgenLock;

//         numDisplayRows = numRows + 1;

//         rectWH = {
//             x,
//             y,
//             (numCols * cellW) + ((numCols + 1) * padding),
//             (numDisplayRows * cellH) + ((numDisplayRows + 1) * padding)
//         };

//         rect = makeRectF(rectWH);
//         absoluteRect = rect;

//         z = _z;
//         name = _name;

//         makeUiElts();
//     }

//     void makeUiElts() {
//         container = new ContainerElt(gfx, makeRectF(0, 0, rectWH.w, rectWH.h), true);
//         pushChild(container);

//         grid = new GridElt(gfx, 0, 0, numDisplayRows, numCols, cellW, cellH, padding);
//         container->pushChild(grid);

//         makeTransport();
//         makeSeqGrid();
//     }

//     void makeTransport() {
//         int row = 0;

//         for (int col = 0; col < numCols; ++col) {
//             ToggleButtonElt* elt = new ToggleButtonElt(
//                 gfx,
//                 inputState,
//                 makeRectF(0, 0, cellW, cellH),
//                 white,
//                 gray,
//                 green
//             );

//             grid->pushElt(row, col, elt);
//         }
//     }

//     void makeSeqGrid() {
//         for (int row = 0; row < numRows; ++row) {
//             for (int col = 0; col < numCols; ++col) {
//                 ToggleButtonElt* button = new ToggleButtonElt(
//                     gfx,
//                     inputState,
//                     makeRectF(0, 0, cellW, cellH),
//                     white,
//                     gray,
//                     green
//                 );

//                 button->isToggled = false;

//                 button->onLeftClick = [=](int x, int y) {
//                     selectedRow = row;
//                     selectedCol = col;
//                     button->toggleIsSelected();
//                     button->isToggled = !button->isToggled;
//                 };

//                 grid->pushElt(row + 1, col, button);
//             }
//         }
//     }

//     void setSelected(int row, int col) {
//         selectedRow = row;
//         selectedCol = col;
//     }
// };

class ValueSeqGridElt : public BaseElt {
public:
    SharedData* sharedData = nullptr;
    BaseElt* container = nullptr;
    GridElt* grid = nullptr;
    std::mutex* rootUgenLock;

    RectWH rectWH;

    int numRows = 2;
    int numDisplayRows = 0;
    int numCols = 5;
    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    int selectedRow = -1;
    int selectedCol = -1;

    ValueSeqGridElt(
        GraphicsService* _gfx,
        InputState* _inputState,
        SharedData* _sharedData,
        int x,
        int y,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        inputState = _inputState;
        sharedData = _sharedData;
        rootUgenLock = &sharedData->rootUgenLock;

        numDisplayRows = numRows + 1;

        rectWH = {
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding),
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
        container->z = -99;
        pushChild(container);

        // grid = new GridElt(gfx, 0, 0, numDisplayRows, numCols, cellW, cellH, padding);
        // container->pushChild(grid);

        makeTransport();
        makeSeqGrid();
    }

    void makeTransport() {
        int row = 0;

        int curX = padding;
        int curY = padding;

        for (int col = 0; col < numCols; ++col) {
            ToggleButtonElt* elt = new ToggleButtonElt(
                gfx,
                inputState,
                makeRectF(curX, curY, cellW, cellH),
                white,
                gray,
                green
            );
            elt->z = 5;

            if (col == 0) {
                elt->name = "weird";
            }

            container->pushChild(elt);

            curX += padding + cellW;
        }
    }

    void makeSeqGrid() {
        int curX = padding;
        int curY = padding + cellH + padding;

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
                button->z = 5;

                button->isToggled = false;

                button->onLeftClick = [=](int x, int y) {
                    selectedRow = row;
                    selectedCol = col;
                    button->toggleIsSelected();
                    button->isToggled = !button->isToggled;
                };

                container->pushChild(button);

                curX += padding + cellW;
            }

            curX = padding;
            curY += padding + cellH;
        }
    }

    void setSelected(int row, int col) {
        selectedRow = row;
        selectedCol = col;
    }
};
