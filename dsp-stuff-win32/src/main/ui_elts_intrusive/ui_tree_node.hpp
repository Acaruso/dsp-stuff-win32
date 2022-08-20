#pragma once

#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/util.hpp"

struct UiTreeNode {
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;
    D2D1_RECT_F absoluteRect;
    UiTreeNode* parent = nullptr;
    std::vector<UiTreeNode*> children;

    void (*onLeftClick)(UiTreeNode* uiTreeNode, int x, int y) = [](UiTreeNode* uiTreeNode, int x, int y) {};
    void (*onLeftDrag)(UiTreeNode* uiTreeNode, int x, int y, int xDelta, int yDelta) = [](UiTreeNode* uiTreeNode, int x, int y, int xDelta, int yDelta) {};
    void (*onMouseWheel)(UiTreeNode* uiTreeNode, int wheelDelta) = [](UiTreeNode* uiTreeNode, int wheelDelta) {};
    void (*onKeyDown)(UiTreeNode* uiTreeNode, int keyCode) = [](UiTreeNode* uiTreeNode, int keyCode) {};
    void (*onDraw)(UiTreeNode* uiTreeNode) = [](UiTreeNode* uiTreeNode) {};

    void pushChild(UiTreeNode* child) {
        child->setParent(this);
        children.push_back(child);
    }

    void setParent(UiTreeNode* parent) {
        this->parent = parent;
        this->absoluteRect = makeOffsetRect(rect, parent->rect.left, parent->rect.top);
    }

    void handleDraw() {
        onDraw(this);

        gfx->pushOffset(rect);

        for (auto child : children) {
            child->handleDraw();
        }

        gfx->popOffset();
    }
};
