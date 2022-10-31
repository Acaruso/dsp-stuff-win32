#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/audio/ugens/seqs/trigger_seq.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/trigger_seq_grid_elt.hpp"
#include "src/main/ui_elts/advanced/value_seq_grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class GridScreen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    UgenManager* rootUgen = nullptr;
    UgenCtx* ugenCtx = nullptr;
    std::mutex* rootUgenLock;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState,
        BaseElt* _uiRoot,
        UiCompositeFactory* _uiCompositeFactory
    ) override {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = _uiRoot;
        uiCompositeFactory = _uiCompositeFactory;
        rootUgen = &sharedData->rootUgen;
        ugenCtx = rootUgen->ugenCtx;
        rootUgenLock = &sharedData->rootUgenLock;

        makeUiControls();
    }

    void makeUiControls() {
        BaseElt* seqGrid = new ValueSeqGridElt(gfx, inputState, sharedData, 10, 10);
        uiRoot->pushChild(seqGrid);
    }
};
