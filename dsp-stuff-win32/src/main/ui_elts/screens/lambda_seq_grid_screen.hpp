#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/lambda_seq.hpp"
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
#include "src/main/ui_elts/composite/ugen_ui_elt_factory.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class LambdaSeqGridScreen : public BaseScreen {
public:
    GraphicsService* gfx;
    SharedData* sharedData;
    InputState* inputState;
    BaseElt* uiRoot;
    UiEltFactory* uiEltFactory;
    UgenUiEltFactory* ugenUiEltFactory;
    UgenManager* rootUgen;
    UgenCtx* ugenCtx;
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
        ugenUiEltFactory = new UgenUiEltFactory(gfx, sharedData, inputState, uiEltFactory);

        rootUgen = &sharedData->rootUgen;
        ugenCtx = rootUgen->ugenCtx;
        rootUgenLock = &sharedData->rootUgenLock;

        rootUgenLock->lock();
        makeUgens();
        makeUiControls();
        rootUgenLock->unlock();
    }

    void makeUgens() {
        float level = 0.2f;

        int kick = rootUgen->addUgen(
            "kick",
            makeSinOscEnvFreqEnv(
                ugenCtx,
                { 0.0f, 200.0f, 10.0f },
                { 0.0f, 0.0f, 50.0f, 60.0f, 400.0f },
                level
            )
        );

        int snare = rootUgen->addUgen(
            "snare",
            makeWhiteNoiseOscEnv(
                ugenCtx,
                { 0.0f, 80.0f, 180.0f },
                level
            )
        );

        int bass = rootUgen->addUgen(
            "bass",
            makeTwoOp(
                ugenCtx,
                { 0.0f, 180.0f, 180.0f },
                { 100.0f, 20.0f, 80.0f, 0.0f, 4.0f },
                level
            )
        );

        int hiHat = rootUgen->addUgen(
            "hiHat",
            makeWavetableOscEnvFreqEnv(
                ugenCtx,
                ugenCtx->wavetables.noise,
                { 0.0f, 10.0f, 0.0f },
                { 0.0f, 4.0f, 20.0f },
                2,
                400,
                level
            )
        );

        // create seq
        int seq = rootUgen->addUgen("seq", makeSeq(5));
        int t2c = rootUgen->addUgen(new TrigToConstValue(ugenCtx, 0.0f));
        int seqSplit = rootUgen->addUgen(new Split(ugenCtx, 2));

        // add 4 ins to outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);
        pOutSum->addIns(4);

        // connect ugens
        rootUgen->connect(
            std::vector<int> {
                seq,      0,    kick,     0,
                seq,      1,    snare,    0,
                seq,      2,    bass,     0,
                seq,      3,    t2c,      0,
                t2c,      0,    seqSplit, 0,
                seqSplit, 0,    bass,     1,
                seqSplit, 1,    bass,     2,
                seq,      4,    hiHat,    0,
                kick,     0,    outSum,   0,
                snare,    0,    outSum,   1,
                bass,     0,    outSum,   2,
                hiHat,    0,    outSum,   3,
            }
        );
    }

    BaseUgen* makeSeq(int numTracks) {
        LambdaSeq* p_seq = new LambdaSeq(ugenCtx, 6200, numTracks);

        UgenManager* p_kick = (UgenManager*)rootUgen->getUgen("kick");
        AHRExpEnvScale* p_freq = (AHRExpEnvScale*)(p_kick->getUgen("freqEnv"));

        std::function<void(int trackIdx, int stepIdx, LambdaSeqCell& cell)> kickLambda = [=](
            int trackIdx,
            int stepIdx,
            LambdaSeqCell& cell
        ) {
            double r = getRand();
            if (r <= 0.10) {
                p_freq->setAttack(100);
            } else {
                p_freq->setAttack(0);
            }

            r = getRand();
            if (r <= 0.40) {
                if (r < 0.20) {
                    cell.value = 0.0f;
                }
                auto& track = p_seq->getTrack(trackIdx);
                track[stepIdx + 1].on = true;
                track[stepIdx + 1].value = 1.0f;
            } else {
                cell.value = 1.0f;
                auto& track = p_seq->getTrack(trackIdx);
                track[stepIdx + 1].on = false;
                track[stepIdx + 1].value = 0.0f;
            }
        };

        // track 0 - kick
        p_seq->set(0, 0, kickLambda);
        p_seq->set(0, 4, kickLambda);
        p_seq->set(0, 8, kickLambda);
        p_seq->set(0, 12, kickLambda);

        // track 1 - snare
        p_seq->set(1, 4);
        p_seq->set(1, 12);

        // track 2 and 3 - bass
        p_seq->set(2, 2);
        p_seq->set(3, 2, 50);

        p_seq->set(2, 6);
        p_seq->set(3, 6, 100);

        p_seq->set(2, 10);
        p_seq->set(3, 10, 75);

        p_seq->set(2, 11);
        p_seq->set(3, 11, 275);

        p_seq->set(2, 13);
        p_seq->set(3, 13, 475);

        p_seq->set(3, 14, 875);
        p_seq->set(3, 15, 1175);

        UgenManager* p_hiHat = (UgenManager*)rootUgen->getUgen("hiHat");
        AHRExpEnv* p_HHAmp = (AHRExpEnv*)(p_hiHat->getUgen("ampEnv"));
        AHRExpEnv* p_HHFreq = (AHRExpEnv*)(p_hiHat->getUgen("freqEnv"));

        std::function<void(int trackIdx, int stepIdx, LambdaSeqCell& cell)> hiHatLambda = [=](
            int trackIdx,
            int stepIdx,
            LambdaSeqCell& cell
        ) {
            double r = getRand();
            if (r <= 0.40 && p_seq->stepIdx % 2 != 0) {
                p_HHAmp->setHold(50);
                p_HHFreq->setAttack(40);
                p_HHFreq->setHold(10);
                p_HHFreq->setRelease(50);
            } else {
                p_HHAmp->setHold(10);
                p_HHFreq->setAttack(0);
                p_HHFreq->setHold(4);
                p_HHFreq->setRelease(10);
            }
        };

        for (int i = 0; i < 16; i++) {
            p_seq->set(4, i, hiHatLambda);
        }

        return p_seq;
    }

    void makeUiControls() {
        makeSeqControls();
        makeUgenControls();
    }

    void makeSeqControls() {
        LambdaSeq* p_seq = (LambdaSeq*)rootUgen->getUgen("seq");

        // make seq grid

        uiRoot->pushChild(
            new LambdaSeqGridElt(
                gfx,
                inputState,
                sharedData,
                uiEltFactory,
                p_seq,
                10,
                10
            )
        );

        // make play button

        uiRoot->pushChild(
            uiEltFactory->makeButtonAndLabel(
                L"Play",
                900,
                200,
                [=](int x, int y) {
                    rootUgenLock->lock();
                    p_seq->toggle();
                    rootUgenLock->unlock();
                }
            )
        );

        // make len16 number

        uiRoot->pushChild(
            uiEltFactory->makeNumberAndLabel(
                L"Len16",
                p_seq->n16len,
                1,
                100000,
                980,
                200,
                [=](int newNumber) { p_seq->n16len = newNumber; }
            )
        );
    }

    void makeUgenControls() {
        // make kick controls

        ContainerElt* kickEnvControls = (ContainerElt*)uiRoot->pushChild(
            makeKickControls(
                (UgenManager*)rootUgen->getUgen("kick"),
                400,
                360
            )
        );

        // make snare controls

        ContainerElt* snareEnvControls = (ContainerElt*)uiRoot->pushChild(
            makeSnareControls(
                (UgenManager*)rootUgen->getUgen("snare"),
                400,
                360
            )
        );

        snareEnvControls->visible = false;

        // make bass controls

        ContainerElt* bassEnvControls = (ContainerElt*)uiRoot->pushChild(
            makeBassControls(
                (UgenManager*)rootUgen->getUgen("bass"),
                400,
                360
            )
        );

        bassEnvControls->visible = false;

        // make kick button

        TextButtonElt* kickButton = (TextButtonElt*)uiRoot->pushChild(
            new TextButtonElt(gfx, inputState, L"Kick", 400, 340)
        );

        kickButton->onLeftClick = [=](int x, int y) {
            kickEnvControls->visible = true;
            snareEnvControls->visible = false;
            bassEnvControls->visible = false;
        };

        // make snare button

        TextButtonElt* snareButton = (TextButtonElt*)uiRoot->pushChild(
            new TextButtonElt(gfx, inputState, L"Snare", 450, 340)
        );

        snareButton->onLeftClick = [=](int x, int y) {
            kickEnvControls->visible = false;
            snareEnvControls->visible = true;
            bassEnvControls->visible = false;
        };

        // make bass button

        TextButtonElt* bassButton = (TextButtonElt*)uiRoot->pushChild(
            new TextButtonElt(gfx, inputState, L"Bass", 500, 340)
        );

        bassButton->onLeftClick = [=](int x, int y) {
            kickEnvControls->visible = false;
            snareEnvControls->visible = false;
            bassEnvControls->visible = true;
        };
    }

    ContainerElt* makeKickControls(UgenManager* p_kick, int x, int y) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 500, 160 },
            true
        );

        AHRExpEnvVca* p_amp = (AHRExpEnvVca*)p_kick->getUgen("ampEnv");
        AHRExpEnvScale* p_freq = (AHRExpEnvScale*)p_kick->getUgen("freqEnv");

        container->pushChild(
            ugenUiEltFactory->makeAHREnvControls(
                L"Amp",
                p_amp,
                10,
                10
            )
        );

        container->pushChild(
            ugenUiEltFactory->makeAHREnvScaleControls(
                L"Freq",
                p_freq,
                200,
                10
            )
        );

        return container;
    }

    ContainerElt* makeSnareControls(UgenManager* p_snare, int x, int y) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 500, 160 },
            true
        );

        AHRExpEnv* p_amp = (AHRExpEnv*)p_snare->getUgen("ampEnv");

        container->pushChild(
            ugenUiEltFactory->makeAHREnvControls(
                L"Amp",
                p_amp,
                10,
                10
            )
        );

        return container;
    }

    ContainerElt* makeBassControls(UgenManager* p_bass, int x, int y) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 500, 160 },
            true
        );

        AHRExpEnvVca* p_amp = (AHRExpEnvVca*)p_bass->getUgen("ampEnv");
        AHRExpEnvVcaScale* p_mod = (AHRExpEnvVcaScale*)p_bass->getUgen("modEnv");

        container->pushChild(
            ugenUiEltFactory->makeAHREnvControls(
                L"Amp",
                p_amp,
                10,
                10
            )
        );

        container->pushChild(
            ugenUiEltFactory->makeAHREnvScaleControls(
                L"Mod",
                p_mod,
                200,
                10
            )
        );

        return container;
    }
};
