#pragma once

#include <functional>
#include <string>
#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/input_state.hpp"
#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    InputState* inputState = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F absoluteRect;
    int z = 0;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    std::string name;

    std::function<void(int x, int y)> onLeftClick = [](int x, int y) {};
    std::function<void(int x, int y, int xDelta, int yDelta)> onLeftDrag = [](int x, int y, int xDelta, int yDelta) {};
    std::function<void(int wheelDelta)> onMouseWheel = [](int wheelDelta) {};
    std::function<void(int keyCode)> onKeyDown = [](int keyCode) {};

    void pushChild(BaseElt* child) {
        child->setParent(this);
        children.push_back(child);
    }

    void setParent(BaseElt* _parent) {
        parent = _parent;
        updateAbsoluteRect();
    }

    void updateAbsoluteRect() {
        absoluteRect = makeOffsetRect(
            rect, 
            (int)parent->absoluteRect.left, 
            (int)parent->absoluteRect.top
        );

        for (auto child : children) {
            child->updateAbsoluteRect();
        }
    }

    virtual void onDraw() {}

    virtual void onTick() {}

    virtual ~BaseElt() = default;
};
