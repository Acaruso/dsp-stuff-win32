#pragma once

#include <functional>
#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/util.hpp"

class BaseElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F absoluteRect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;

    std::function<void(int x, int y)> onLeftClick = [](int x, int y) {};
    std::function<void(int x, int y, int xDelta, int yDelta)> onLeftDrag = [](int x, int y, int xDelta, int yDelta) {};
    std::function<void(int wheelDelta)> onMouseWheel = [](int wheelDelta) {};
    std::function<void(int keyCode)> onKeyDown = [](int keyCode) {};

    // std::function<void()> onDraw = []() {};
    // std::function<void()> onTick = []() {};

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

    void handleLeftDrag(int x, int y, int xDelta, int yDelta) {
        if (!isInsideRect(x, y, absoluteRect)) {
            return;
        }

        onLeftDrag(
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

        onMouseWheel(wheelDelta);

        for (auto& child : children) {
            child->handleMouseWheel(inputState, wheelDelta);
        }
    }

    void handleKeyDown(InputState& inputState, int keyCode) {
        if (!isInsideRect(inputState.mouseX, inputState.mouseY, absoluteRect)) {
            return;
        }

        onKeyDown(keyCode);

        for (auto& child : children) {
            child->handleKeyDown(inputState, keyCode);
        }
    }

    void handleDraw() {
        onDraw();

        gfx->pushOffset(rect);

        for (auto& child : children) {
            child->handleDraw();
        }

        gfx->popOffset();
    }

    void handleTick(InputState& inputState) {
        if (!isInsideRect(inputState.mouseX, inputState.mouseY, absoluteRect)) {
            return;
        }

        onTick();

        for (auto& child : children) {
            child->handleTick(inputState);
        }
    }

    virtual void onDraw() {}

    virtual void onTick() {}

    virtual ~BaseElt() = default;
};
