#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/seqs/trigger_seq.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/toggle_button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class GridElt : public BaseElt {
public:
    BaseElt* container = nullptr;
    std::vector<std::vector<BaseElt*>> grid;
    int numRows = 0;
    int numCols = 0;
    int cellW = 30;
    int cellH = 30;
    int padding = 5;

    GridElt(
        GraphicsService* _gfx,
        int x,
        int y,
        int _numRows,
        int _numCols,
        int _cellW,
        int _cellH,
        int _padding,
        int _z=0,
        std::string _name=""
    ) {
        gfx = _gfx;
        numRows = _numRows;
        numCols = _numCols;
        cellW = _cellW;
        cellH = _cellH;
        padding = _padding;

        grid.resize(numRows, std::vector<BaseElt*>(numCols, nullptr));

        setRects({
            x,
            y,
            (numCols * cellW) + ((numCols + 1) * padding),
            (numRows * cellH) + ((numRows + 1) * padding)
        });

        z = _z;
        name = _name;

        container = new ContainerElt(gfx, { 0, 0, rectWH.w, rectWH.h });
        pushChild(container);
    }

    void pushElt(int row, int col, BaseElt* elt) {
        int x = padding + (col * cellW) + (col * padding);
        int y = padding + (row * cellH) + (row * padding);

        elt->setRects({ x, y, elt->rectWH.w, elt->rectWH.h });

        container->pushChild(elt);

        grid[row][col] = elt;
    }

    BaseElt* getElt(int row, int col) {
        return grid[row][col];
    }
};
