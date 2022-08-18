#pragma once

#include <functional>
#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/util.hpp"

class BaseEltNew {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F absoluteRect;
    BaseEltNew* parent = nullptr;
    std::vector<BaseEltNew*> children;

    void* data = nullptr;

    std::function<void(BaseEltNew* pThis, int x, int y)> onLeftClick = [](BaseEltNew* pThis, int x, int y) {};
    std::function<void(BaseEltNew* pThis, int x, int y, int xDelta, int yDelta)> onLeftDrag = [](BaseEltNew* pThis, int x, int y, int xDelta, int yDelta) {};
    std::function<void(BaseEltNew* pThis, int wheelDelta)> onMouseWheel = [](BaseEltNew* pThis, int wheelDelta) {};
    std::function<void(BaseEltNew* pThis, int keyCode)> onKeyDown = [](BaseEltNew* pThis, int keyCode) {};
    std::function<void(BaseEltNew* pThis)> onTick = [](BaseEltNew* pThis) {};
    std::function<void(BaseEltNew* pThis)> onDraw = [](BaseEltNew* pThis) {};

    BaseEltNew(GraphicsService* gfx_, D2D1_RECT_F rect_) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;
    }

    void pushChild(BaseEltNew* child) {
        child->setParent(this);
        children.push_back(child);
    }

    void setParent(BaseEltNew* parent_) {
        parent = parent_;
        absoluteRect = makeOffsetRect(rect, parent->rect.left, parent->rect.top);
    }

    void handleLeftClick(int x, int y) {
        if (!isInsideRect(x, y, absoluteRect)) {
            return;
        }

        onLeftClick(this, x - absoluteRect.left, y - absoluteRect.top);

        for (auto& child : children) {
            child->handleLeftClick(x, y);
        }
    }

    void handleLeftDrag(int x, int y, int xDelta, int yDelta) {
        if (!isInsideRect(x, y, absoluteRect)) {
            return;
        }

        onLeftDrag(
            this,
            x - absoluteRect.left,
            y - absoluteRect.top,
            xDelta - absoluteRect.left,
            yDelta - absoluteRect.top
        );

        for (auto& child : children) {
            child->handleLeftDrag(x, y, xDelta, yDelta);
        }
    }

    void handleMouseWheel(InputState& inputState, int wheelDelta) {
        if (!isInsideRect(inputState.mouseX, inputState.mouseY, absoluteRect)) {
            return;
        }

        onMouseWheel(this, wheelDelta);

        for (auto& child : children) {
            child->handleMouseWheel(inputState, wheelDelta);
        }
    }

    void handleKeyDown(InputState& inputState, int keyCode) {
        if (!isInsideRect(inputState.mouseX, inputState.mouseY, absoluteRect)) {
            return;
        }

        onKeyDown(this, keyCode);

        for (auto& child : children) {
            child->handleKeyDown(inputState, keyCode);
        }
    }

    void handleTick(InputState& inputState) {
        if (!isInsideRect(inputState.mouseX, inputState.mouseY, absoluteRect)) {
            return;
        }

        onTick(this);

        for (auto& child : children) {
            child->handleTick(inputState);
        }
    }

    void handleDraw() {
        onDraw(this);

        gfx->pushOffset(rect);

        for (auto& child : children) {
            child->handleDraw();
        }

        gfx->popOffset();
    }
};
