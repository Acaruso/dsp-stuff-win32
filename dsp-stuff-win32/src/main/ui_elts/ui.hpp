#pragma once

#include <deque>
#include <vector>

#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/main/ui_elts/screens/complex_screen/complex_screen.hpp"
#include "src/main/ui_elts/screens/lambda_seq_grid_screen.hpp"
#include "src/main/ui_elts/screens/simple_screen.hpp"
#include "src/main/ui_elts/screens/trigger_seq_grid_screen.hpp"
#include "src/main/ui_elts/screens/two_op_two_freq_env_screen.hpp"
#include "src/main/ui_elts/screens/value_seq_grid_screen.hpp"
#include "src/main/ui_elts/screens/waveshaper_screen/waveshaper_screen.hpp"
#include "src/shared/shared_data.hpp"

class Ui {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    std::vector<BaseElt*> curLeftClickedElts;

    BaseScreen* simpleScreen = new SimpleScreen;
    BaseScreen* complexScreen = new ComplexScreen;
    BaseScreen* waveshaperScreen = new WaveshaperScreen;
    BaseScreen* triggerSeqGridScreen = new TriggerSeqGridScreen;
    BaseScreen* valueSeqGridScreen = new ValueSeqGridScreen;
    BaseScreen* lambdaSeqGridScreen = new LambdaSeqGridScreen;
    BaseScreen* twoOpTwoFreqEnvScreen = new TwoOpTwoFreqEnvScreen;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = new ContainerElt(gfx, { 0, 0, windowWidth, windowHeight });
        uiCompositeFactory = new UiCompositeFactory(gfx, inputState, sharedData);
    }

    void initUi() {
        // simpleScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // complexScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // waveshaperScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // triggerSeqGridScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // valueSeqGridScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // lambdaSeqGridScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        twoOpTwoFreqEnvScreen->init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
    }

    void handleLeftMBDown(int x, int y) {
        handleLeftMBDown(uiRoot, x, y);
    }

    inline void handleLeftMBDown(BaseElt* elt, int x, int y) {
        if (!elt->visible) {
            return;
        }

        std::vector<BaseElt*> toLeftClick;

        std::deque<BaseElt*> q;
        q.push_front(elt);

        BaseElt* cur = nullptr;

        while (!q.empty()) {
            cur = q.back();
            q.pop_back();

            if (!cur->visible) {
                continue;
            }

            if (!isInsideRect(x, y, cur->absRect)) {
                continue;
            }

            curLeftClickedElts.push_back(cur);

            toLeftClick.push_back(cur);

            for (auto child : cur->children) {
                q.push_front(child);
            }
        }

        for (auto elt : toLeftClick) {
            elt->_onLeftClick(
                (int)(x - elt->absRect.left),
                (int)(y - elt->absRect.top)
            );
        }
    }

    void handleLeftMBUp(int x, int y) {
        curLeftClickedElts.clear();
    }

    inline void handleLeftMBDrag(int x, int y, int xDelta, int yDelta) {
        for (auto elt : curLeftClickedElts) {
            elt->onLeftDrag(
                (int)(x - elt->absRect.left),
                (int)(y - elt->absRect.top),
                xDelta,
                yDelta
            );
        }
    }

    void handleMouseWheel(int wheelDelta) {
        handleMouseWheel(uiRoot, inputState, wheelDelta);
    }

    inline void handleMouseWheel(BaseElt* elt, InputState* inputState, int wheelDelta) {
        if (!elt->visible) {
            return;
        }

        if (!isInsideRect(inputState->mouseX, inputState->mouseY, elt->absRect)) {
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
        if (!elt->visible) {
            return;
        }

        if (!isInsideRect(inputState->mouseX, inputState->mouseY, elt->absRect)) {
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
        if (!elt->visible) {
            return;
        }

        elt->onDraw();

        gfx->pushOffset(elt->relRect);

        for (auto child : elt->children) {
            handleDraw(gfx, child);
        }

        gfx->popOffset();
    }

    void handleTick() {
        handleTick(uiRoot);
    }

    inline void handleTick(BaseElt* elt) {
        if (!elt->visible) {
            return;
        }

        elt->onTick();

        for (auto child : elt->children) {
            handleTick(child);
        }
    }
};
