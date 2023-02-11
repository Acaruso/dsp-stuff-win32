#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/lambda_seq.hpp"
#include "src/audio/ugens/seqs/value_seq.hpp"
#include "src/audio/ugens/song2/song2_main.hpp"
#include "src/audio/ugens/song2/song2_main2.hpp"
#include "src/audio/ugens/trig_to_const_value.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/lambda_seq_grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/float_number_elt.hpp"
#include "src/main/ui_elts/basic/note_number_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/basic/text_button_elt.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class Song2Screen : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiEltFactory* uiEltFactory = nullptr;
    UgenManager* rootUgen = nullptr;
    UgenCtx* ugenCtx = nullptr;
    std::mutex* rootUgenLock;

    void init(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState,
        BaseElt* _uiRoot
    ) override {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiRoot = _uiRoot;
        uiEltFactory = new UiEltFactory(gfx, inputState, sharedData);
        
        rootUgen = &sharedData->rootUgen;
        ugenCtx = rootUgen->ugenCtx;
        rootUgenLock = &sharedData->rootUgenLock;

        rootUgenLock->lock();
        makeUgens();
        makeUiControls();
        rootUgenLock->unlock();
    }

    // void makeUgens() {
    //     UgenManager* root = &sharedData->rootUgen;

    //     int song2Main = root->addUgen(
    //         new Song2::Main(
    //             &sharedData->ugenCtx,
    //             sharedData->ugenCtx.wavetables.sin
    //         )
    //     );

    //     int outSum = root->getUgenId("outSum");
    //     BaseUgen* pOutSum = root->getUgen(outSum);
    //     pOutSum->addIn();
    //     root->connect(song2Main, 0, outSum, 0);
    // }

    void makeUgens() {
        UgenManager* root = &sharedData->rootUgen;

        int song2Main2 = root->addUgen(
            new Song2::Main2(
                &sharedData->ugenCtx,
                sharedData->ugenCtx.wavetables.sin
            )
        );

        int outSum = root->getUgenId("outSum");
        BaseUgen* pOutSum = root->getUgen(outSum);
        pOutSum->addIn();
        root->connect(song2Main2, 0, outSum, 0);
    }

    void makeUiControls() {
    }
};
