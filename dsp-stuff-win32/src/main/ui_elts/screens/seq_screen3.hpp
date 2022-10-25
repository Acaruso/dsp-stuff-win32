#pragma once

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/seqs/basic_seq.hpp"
#include "src/audio/ugens/seqs/pattern_seq.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/number_elt.hpp"
#include "src/main/ui_elts/composite/ui_composite_factory.hpp"
#include "src/main/ui_elts/screens/base_screen.hpp"
#include "src/shared/shared_data.hpp"

class SeqScreen3 : public BaseScreen {
public:
    GraphicsService* gfx = nullptr;
    SharedData* sharedData = nullptr;
    InputState* inputState = nullptr;
    BaseElt* uiRoot = nullptr;
    UiCompositeFactory* uiCompositeFactory = nullptr;
    UgenManager* rootUgen = nullptr;
    UgenCtx* ugenCtx = nullptr;
    std::mutex* rootUgenLock;
    int numOscs = 0;

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

        makeUgens();
    }

    void makeUgens() {
        rootUgenLock->lock();

        // create kick
        UgenManager* pKick = makeSinOscEnvFreqEnv(
            ugenCtx,
            AHRData{1.0f, 200.0f, 10.0f},
            AHRData{0.1f, 1.0f, 100.0f},
            60,
            400,
            0.5f
        );

        int kick = rootUgen->addUgen(pKick);

        // create white noise snare
        UgenManager* pSnare = makeWhiteNoiseOscEnv(
            ugenCtx,
            AHRData{1.0f, 80.0f, 180.0f},
            0.5f
        );

        int snare = rootUgen->addUgen(pSnare);

        // create seq
        PatternSeq* pSeq = new PatternSeq(ugenCtx, 5000);
        int seq = rootUgen->addUgen(pSeq);

        // connect seq out0 to kick in0
        rootUgen->connect(seq, 0, kick, 0);

        // connect seq out1 to snare in0
        rootUgen->connect(seq, 1, snare, 0);

        // get outSum
        int outSum = rootUgen->getUgenId("outSum");
        BaseUgen* pOutSum = rootUgen->getUgen(outSum);

        // connect osc to outSum
        pOutSum->addIn();
        rootUgen->connect(kick, 0, outSum, numOscs);
        ++numOscs;

        // connect snare to outSum
        pOutSum->addIn();
        rootUgen->connect(snare, 0, outSum, numOscs);
        ++numOscs;

        // ui elements //////////////////////////////////////////////////////////

        // create play button
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

        // create len16 number
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

        // amp //////////////////////////////////

        // amp attack
        AHRExpEnv* pAmp = (AHRExpEnv*)(pKick->getUgen("ampEnv"));

        BaseElt* ampAttack = uiCompositeFactory->makeNumberAndLabel(
            L"Amp Attack",
            sampstoms(pAmp->attackSamps),
            0,
            10000,
            200,
            300,
            [=](int newNumber) {
                rootUgenLock->lock();
                pAmp->setAttack(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(ampAttack);

        // amp hold
        BaseElt* ampHold = uiCompositeFactory->makeNumberAndLabel(
            L"Amp Hold",
            sampstoms(pAmp->holdSamps),
            0,
            10000,
            200,
            350,
            [=](int newNumber) {
                rootUgenLock->lock();
                pAmp->setHold(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(ampHold);

        // amp release
        BaseElt* ampRelease = uiCompositeFactory->makeNumberAndLabel(
            L"Amp Release",
            sampstoms(pAmp->releaseSamps),
            0,
            10000,
            200,
            400,
            [=](int newNumber) {
                rootUgenLock->lock();
                pAmp->setRelease(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(ampRelease);

        // freq /////////////////////////////////

        // freq attack
        AHRExpEnv* pFreq = (AHRExpEnv*)(pKick->getUgen("freqEnv"));

        BaseElt* freqAttack = uiCompositeFactory->makeNumberAndLabel(
            L"Freq Attack",
            sampstoms(pFreq->attackSamps),
            0,
            10000,
            360,
            300,
            [=](int newNumber) {
                rootUgenLock->lock();
                pFreq->setAttack(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(freqAttack);

        // freq hold
        BaseElt* freqHold = uiCompositeFactory->makeNumberAndLabel(
            L"Freq Hold",
            sampstoms(pFreq->holdSamps),
            0,
            10000,
            360,
            350,
            [=](int newNumber) {
                rootUgenLock->lock();
                pFreq->setHold(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(freqHold);

        // freq release
        BaseElt* freqRelease = uiCompositeFactory->makeNumberAndLabel(
            L"Freq Release",
            sampstoms(pFreq->releaseSamps),
            0,
            10000,
            360,
            400,
            [=](int newNumber) {
                rootUgenLock->lock();
                pFreq->setRelease(newNumber);
                rootUgenLock->unlock();
            }
        );

        uiRoot->pushChild(freqRelease);

        rootUgenLock->unlock();
    }
};
