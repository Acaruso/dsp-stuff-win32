#pragma once

#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/util.hpp"
#include "src/main/input_state.hpp"

inline void handleLeftClick(BaseElt* elt, int x, int y) {
    if (!isInsideRect(x, y, elt->absoluteRect)) {
        return;
    }

    elt->onLeftClick(x - elt->absoluteRect.left, y - elt->absoluteRect.top);

    for (auto child : elt->children) {
        handleLeftClick(child, x, y);
    }
}

inline void handleLeftDrag(BaseElt* elt, int x, int y, int xDelta, int yDelta) {
    if (!isInsideRect(x, y, elt->absoluteRect)) {
        return;
    }

    elt->onLeftDrag(
        x - elt->absoluteRect.left,
        y - elt->absoluteRect.top,
        xDelta - elt->absoluteRect.left,
        yDelta - elt->absoluteRect.top
    );

    for (auto child : elt->children) {
        handleLeftDrag(child, x, y, xDelta, yDelta);
    }
}

inline void handleMouseWheel(BaseElt* elt, InputState& inputState, int wheelDelta) {
    if (!isInsideRect(inputState.mouseX, inputState.mouseY, elt->absoluteRect)) {
        return;
    }

    elt->onMouseWheel(wheelDelta);

    for (auto child : elt->children) {
        handleMouseWheel(child, inputState, wheelDelta);
    }
}

inline void handleKeyDown(BaseElt* elt, InputState& inputState, int keyCode) {
    if (!isInsideRect(inputState.mouseX, inputState.mouseY, elt->absoluteRect)) {
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
