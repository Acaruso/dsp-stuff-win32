#pragma once

#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/rect_wh.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/complex_screen.hpp"
#include "src/main/ui_elts/screens/simple_screen.hpp"
#include "src/main/ui_elts/ui_elt_util.hpp"
#include "src/shared/shared_data.hpp"

class Ui {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;

    SimpleScreen simpleScreen;
    ComplexScreen complexScreen;

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
        simpleScreen.init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
        // complexScreen.init(gfx, sharedData, inputState, uiRoot, uiCompositeFactory);
    }

    void handleLeftClick(int x, int y) {
        ::handleLeftClick(uiRoot, x, y);
    }

    void handleLeftDrag(int x, int y, int xDelta, int yDelta) {
        ::handleLeftDrag(uiRoot, x, y, xDelta, yDelta);
    }

    void handleMouseWheel(int wheelDelta) {
        ::handleMouseWheel(uiRoot, inputState, wheelDelta);
    }

    void handleKeyDown(int keyCode) {
        ::handleKeyDown(uiRoot, inputState, keyCode);
    }

    void handleDraw() {
        ::handleDraw(gfx, uiRoot);
    }

    void handleTick() {
        ::handleTick(uiRoot);
    }
};
