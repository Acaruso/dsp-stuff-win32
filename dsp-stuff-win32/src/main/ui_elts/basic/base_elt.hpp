#pragma once

#include <functional>
#include <vector>

#include <d2d1.h>

#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F absoluteRect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    std::function<void(int x, int y)> onLeftClick = [](int x, int y) {};

    void pushChild(BaseElt* child) {
        child->setParent(this);
        children.push_back(child);
    }

    void setParent(BaseElt* parent) {
        this->parent = parent;
        this->absoluteRect = makeOffsetRect(rect, parent->rect.left, parent->rect.top);
    }

    void handleLeftClick(int x, int y) {
        if (!isInsideRect(x, y, absoluteRect)) {
            return;
        }

        onLeftClick(x - absoluteRect.left, y - absoluteRect.top);

        for (auto& child : children) {
            child->handleLeftClick(x, y);
        }
    }

    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
