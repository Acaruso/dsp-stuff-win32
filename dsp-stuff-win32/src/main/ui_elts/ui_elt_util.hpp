#pragma once

#include <deque>
#include <vector>

#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"
#include "src/main/input_state.hpp"

// need to do it this way because onLeftClick() can modify UI tree
// so we need to call onLeftClick()s seperately from traversing tree
inline void handleLeftClick(BaseElt* elt, int x, int y) {
    std::vector<BaseElt*> toLeftClick;

    std::deque<BaseElt*> q;
    q.push_front(elt);

    BaseElt* cur = nullptr;

    while (!q.empty()) {
        cur = q.back();
        q.pop_back();

        if (!isInsideRect(x, y, cur->absoluteRect)) {
            continue;
        }

        toLeftClick.push_back(cur);

        for (auto child : cur->children) {
            q.push_front(child);
        }
    }

    for (auto elt : toLeftClick) {
        elt->onLeftClick(
            (int)(x - elt->absoluteRect.left),
            (int)(y - elt->absoluteRect.top)
        );
    }
}

inline void handleLeftDrag(BaseElt* elt, int x, int y, int xDelta, int yDelta) {
    if (!isInsideRect(x, y, elt->absoluteRect)) {
        return;
    }

    elt->onLeftDrag(
        (int)(x - elt->absoluteRect.left),
        (int)(y - elt->absoluteRect.top),
        (int)(xDelta - elt->absoluteRect.left),
        (int)(yDelta - elt->absoluteRect.top)
    );

    for (auto child : elt->children) {
        handleLeftDrag(child, x, y, xDelta, yDelta);
    }
}

inline void handleMouseWheel(BaseElt* elt, InputState* inputState, int wheelDelta) {
    if (!isInsideRect(inputState->mouseX, inputState->mouseY, elt->absoluteRect)) {
        return;
    }

    elt->onMouseWheel(wheelDelta);

    for (auto child : elt->children) {
        handleMouseWheel(child, inputState, wheelDelta);
    }
}

// note that key down events are only directed to elts if the mouse is inside them
// this may not always be what we want
inline void handleKeyDown(BaseElt* elt, InputState* inputState, int keyCode) {
    if (!isInsideRect(inputState->mouseX, inputState->mouseY, elt->absoluteRect)) {
        return;
    }

    elt->onKeyDown(keyCode);

    for (auto child : elt->children) {
        handleKeyDown(child, inputState, keyCode);
    }
}

inline void handleDraw(GraphicsService* gfx, BaseElt* elt) {
    elt->onDraw();

    gfx->pushOffset(elt->rect);

    for (auto child : elt->children) {
        handleDraw(gfx, child);
    }

    gfx->popOffset();
}

inline void handleTick(BaseElt* elt) {
    elt->onTick();

    for (auto child : elt->children) {
        handleTick(child);
    }
}
