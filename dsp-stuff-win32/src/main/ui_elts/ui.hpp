#pragma once

#include <deque>
#include <vector>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/complex_screen.hpp"
#include "src/main/ui_elts/screens/seq_screen.hpp"
#include "src/main/ui_elts/screens/simple_screen.hpp"
#include "src/shared/shared_data.hpp"

class Ui {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    std::vector<BaseElt*> curLeftClickedElts;

    SimpleScreen simpleScreen;
    ComplexScreen complexScreen;
    SeqScreen seqScreen;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = new ContainerElt(gfx, makeRectF(0, 0, windowWidth, windowHeight));
        uiCompositeFactory = new UiCompositeFactory(gfx, inputState, sharedData);
    }

    void initUi() {
        // simpleScreen.init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // complexScreen.init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        seqScreen.init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
    }

    void handleLeftMBDown(int x, int y) {
        handleLeftMBDown(uiRoot, x, y);
    }

    inline void handleLeftMBDown(BaseElt* elt, int x, int y) {
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

            curLeftClickedElts.push_back(cur);

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

    void handleLeftMBUp(int x, int y) {
        curLeftClickedElts.clear();
    }

    inline void handleLeftMBDrag(int x, int y, int xDelta, int yDelta) {
        for (auto elt : curLeftClickedElts) {
            elt->onLeftDrag(
                (int)(x - elt->absoluteRect.left),
                (int)(y - elt->absoluteRect.top),
                xDelta,
                yDelta
            );
        }
    }

    void handleMouseWheel(int wheelDelta) {
        handleMouseWheel(uiRoot, inputState, wheelDelta);
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

    void handleKeyDown(int keyCode) {
        handleKeyDown(uiRoot, inputState, keyCode);
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

    void handleDraw() {
        handleDraw(gfx, uiRoot);
    }

    inline void handleDraw(GraphicsService* gfx, BaseElt* elt) {
        elt->onDraw();

        gfx->pushOffset(elt->rect);

        for (auto child : elt->children) {
            handleDraw(gfx, child);
        }

        gfx->popOffset();
    }

    void handleTick() {
        handleTick(uiRoot);
    }

    inline void handleTick(BaseElt* elt) {
        elt->onTick();

        for (auto child : elt->children) {
            handleTick(child);
        }
    }
};
