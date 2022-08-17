#pragma once

#include <functional>
#include <vector>

#include <d2d1.h>

#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F relativeRect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    std::function<void()> onLeftClick = []() {};

    void pushChild(BaseElt* child) {
        child->parent = this;
        child->relativeRect = makeOffsetRect(child->rect, rect.left, rect.top);
        children.push_back(child);
    }

    void handleLeftClick(int x, int y) {
        if (!isInsideRect(x, y, relativeRect)) {
            return;
        }

        onLeftClick();

        for (auto& child : children) {
            child->handleLeftClick(x, y);
        }
    }

    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
