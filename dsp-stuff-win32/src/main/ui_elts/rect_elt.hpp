#pragma once

#include <memory>
#include <vector>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"
#include "src/main/util.hpp"

class RectElt : public BaseElt {
public:
    GraphicsService& gfx;
    std::vector<std::unique_ptr<BaseElt>> children;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    D2D1_RECT_F rect;

    RectElt(GraphicsService& gfx) : gfx(gfx) {}

    std::vector<std::unique_ptr<BaseElt>>& getChildren() override {
        return children;
    }

    void setRect(int x, int y, int w, int h) {
        rect = makeRectF(x, y, w, h);
    }

    void draw() override {
        gfx.drawRect(rect, black);

        for (auto& child : children) {
            child->draw();
        }
    }
};
