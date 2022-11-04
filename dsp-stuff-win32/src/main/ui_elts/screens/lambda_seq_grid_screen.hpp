#pragma once

#include <mutex>

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/audio/ugens/seqs/lambda_seq.hpp"
#include "src/audio/ugens/seqs/value_seq.hpp"
#include "src/audio/ugens/trig_to_const_value.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/lambda_seq_grid_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class LambdaSeqGridScreen : public BaseScreen {
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

        rootUgenLock->lock();
        makeUgens();
        makeUiControls();
        rootUgenLock->unlock();
    }

    void makeUgens() {
        float level = 0.3f;

        // create kick
        UgenManager* pKick = makeSinOscEnvFreqEnv(
            ugenCtx,
            AHRData{0.0f, 200.0f, 10.0f},
            AHRData{0.0f, 0.0f, 50.0f},
            60,
            400,
            level
        );

        int kick = rootUgen->addUgen("kick", pKick);

        // create white noise snare
        UgenManager* pSnare = makeWhiteNoiseOscEnv(
            ugenCtx,
            AHRData{1.0f, 80.0f, 180.0f},
            level
        );

        int snare = rootUgen->addUgen("snare", pSnare);

        // create bass

        UgenManager* pBass = makeTwoOp(
            ugenCtx,
            AHRData{0.0f, 180.0f, 180.0f},
            AHRData{0.0f, 20.0f, 80.0f},
            level
        );

        int bass = rootUgen->addUgen("bass", pBass);

        BaseUgen* pSeq = makeSeq();

        int seq = rootUgen->addUgen("seq", pSeq);

        int t2c = rootUgen->addUgen(new TrigToConstValue(ugenCtx, 0.0f));

        int seqSplit = rootUgen->addUgen(new Split(ugenCtx, 2));

        // get outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);
        pOutSum->addIns(3);

        rootUgen->connect(
            std::vector<int> {
                seq,      0,    kick,     0,
                seq,      1,    snare,    0,
                seq,      2,    bass,     0,
                seq,      3,    t2c,      0,
                t2c,      0,    seqSplit, 0,
                seqSplit, 0,    bass,     1,
                seqSplit, 1,    bass,     2,
                kick,     0,    outSum,   0,
                snare,    0,    outSum,   1,
                bass,     0,    outSum,   2
            }
        );
    }

    BaseUgen* makeSeq() {
        LambdaSeq* pSeq = new LambdaSeq(ugenCtx, 6200, 4);
        
        UgenManager* pKick = (UgenManager*)rootUgen->getUgen("kick");
        AHRExpEnv* pFreq = (AHRExpEnv*)(pKick->getUgen("freqEnv"));

        std::function<void()> kickLambda = [=]() {
            double r = getRand();
            if (r <= 0.10) {
                pFreq->setAttack(100);
            } else {
                pFreq->setAttack(0);
            }
        };

        // track 0 - kick
        pSeq->set(0, 0, kickLambda);
        pSeq->set(0, 4, kickLambda);
        pSeq->set(0, 8, kickLambda);
        pSeq->set(0, 12, kickLambda);

        // track 1 - snare
        pSeq->set(1, 4);
        pSeq->set(1, 12);

        // track 2 and 3 - bass
        pSeq->set(2, 2);
        pSeq->set(3, 2, 50);

        pSeq->set(2, 6);
        pSeq->set(3, 6, 100);

        pSeq->set(2, 10);
        pSeq->set(3, 10, 75);

        pSeq->set(2, 11);
        pSeq->set(3, 11, 275);

        pSeq->set(2, 13);
        pSeq->set(3, 13, 475);

        pSeq->set(3, 14, 875);
        pSeq->set(3, 15, 1175);

        return pSeq;
    }

    void makeUiControls() {
        LambdaSeq* pSeq = (LambdaSeq*)rootUgen->getUgen("seq");

        // grid
        BaseElt* seqGrid = new LambdaSeqGridElt(
            gfx,
            inputState,
            sharedData,
            uiCompositeFactory,
            pSeq,
            10,
            10
        );

        uiRoot->pushChild(seqGrid);

        // play button
        BaseElt* playButton = uiCompositeFactory->makeButtonAndLabel(
            L"Play",
            900,
            200,
            [=](int x, int y) {
                rootUgenLock->lock();
                pSeq->toggle();
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(playButton);

        // len16 number
        BaseElt* period = uiCompositeFactory->makeNumberAndLabel(
            L"Len16",
            pSeq->n16len,
            1,
            100000,
            980,
            200,
            [=](int newNumber) { pSeq->n16len = newNumber; }
        );

        uiRoot->pushChild(period);

        // kick amp env and freq env controls
        UgenManager* pKick = (UgenManager*)rootUgen->getUgen("kick");

        AHRExpEnv* pAmp = (AHRExpEnv*)(pKick->getUgen("ampEnv"));
        makeEnvControls(L"Amp", pAmp, 200, 300);

        AHRExpEnv* pFreq = (AHRExpEnv*)(pKick->getUgen("freqEnv"));
        makeEnvControls(L"Freq", pFreq, 360, 300);
    }

    void makeEnvControls(std::wstring prefix, AHRExpEnv* pEnv, int x, int y) {
        uiRoot->pushChild(
            uiCompositeFactory->makeNumberAndLabel(
                prefix + L" Attack",
                sampstoms(pEnv->attackSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    pEnv->setAttack(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        uiRoot->pushChild(
            uiCompositeFactory->makeNumberAndLabel(
                prefix + L" Hold",
                sampstoms(pEnv->holdSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    pEnv->setHold(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        uiRoot->pushChild(
            uiCompositeFactory->makeNumberAndLabel(
                prefix + L" Release",
                sampstoms(pEnv->releaseSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    pEnv->setRelease(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );
    }
};
