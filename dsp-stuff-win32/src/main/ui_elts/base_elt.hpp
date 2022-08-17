#pragma once

#include <functional>
#include <vector>

#include <d2d1.h>

#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    std::function<void()> onLeftClick = nullptr;

    void pushChild(BaseElt* child) {
        child->parent = this;
        children.push_back(child);
    }

    void handleLeftClick(int x, int y) {
        onLeftClick();

        for (auto& child : children) {
            if (isInsideRect(x, y, child->rect)) {
                child->handleLeftClick(x, y);
            }
        }
    }

    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
