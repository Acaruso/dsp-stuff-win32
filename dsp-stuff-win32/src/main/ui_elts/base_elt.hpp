#pragma once

#include <vector>

#include <d2d1.h>

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;

    void pushChild(BaseElt* child) {
        child->parent = this;
        children.push_back(child);
    }

    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
