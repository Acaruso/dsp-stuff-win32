#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/input_state.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    InputState* inputState = nullptr;
    RectWH rect;
    D2D1_RECT_F relRect;
    D2D1_RECT_F absRect;
    int z = 0;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    std::string name;
    uint64_t data;
    bool visible = true;

    std::function<void(int x, int y)> onLeftClick = [](int x, int y) {};
    std::function<void(int x, int y, int xDelta, int yDelta)> onLeftDrag = [](int x, int y, int xDelta, int yDelta) {};
    std::function<void(int wheelDelta)> onMouseWheel = [](int wheelDelta) {};
    std::function<void(int keyCode)> onKeyDown = [](int keyCode) {};

    BaseElt* pushChild(BaseElt* child) {
        child->setParent(this);
        children.push_back(child);
        return child;
    }

    void setRects(RectWH _rect) {
        rect = _rect;
        relRect = makeRectF(_rect);
        absRect = relRect;
    }

    void setParent(BaseElt* _parent) {
        parent = _parent;
        updateAbsoluteRect();
    }

    void updateAbsoluteRect() {
        absRect = makeOffsetRect(
            relRect, 
            (int)parent->absRect.left, 
            (int)parent->absRect.top
        );

        for (auto child : children) {
            child->updateAbsoluteRect();
        }
    }

    BaseElt* getElt(std::string name) {
        std::vector<BaseElt*> v;
        v.push_back(this);
        BaseElt* cur = nullptr;

        while (!v.empty()) {
            cur = v.back();
            v.pop_back();
            if (cur->name == name) {
                return cur;
            } else {
                for (auto& child : cur->children) {
                    v.push_back(child);
                }
            }
        }

        return nullptr;
    }

    virtual void _onLeftClick(int x, int y) {
        onLeftClick(x, y);
    }

    virtual void onDraw() {}

    virtual void onTick() {}

    virtual ~BaseElt() = default;
};
