#pragma once

#include <memory>
#include <vector>

#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"

class ContainerElt : public BaseElt {
public:
    GraphicsService& gfx;
    std::vector<std::unique_ptr<BaseElt>> children;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    ContainerElt(GraphicsService& gfx) : gfx(gfx) {}

    std::vector<std::unique_ptr<BaseElt>>& getChildren() override {
        return children;
    }

    void draw() override {
        for (auto& child : children) {
            child->draw();
        }
    }
};
