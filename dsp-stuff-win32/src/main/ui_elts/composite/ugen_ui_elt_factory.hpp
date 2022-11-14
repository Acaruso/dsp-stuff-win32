#pragma once

#include <string>

#include "src/audio/ugens/advanced/ahr_exp_env_vca.hpp"
#include "src/audio/ugens/advanced/ahr_exp_env_vca_scale.hpp"
#include "src/audio/ugens/ahr_exp_env.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/composite/ui_elt_factory.hpp"
#include "src/shared/shared_data.hpp"

class UgenUiEltFactory {
    GraphicsService* gfx;
    SharedData* sharedData;
    InputState* inputState;
    UiEltFactory* uiEltFactory;
    std::mutex* rootUgenLock;

public:
    UgenUiEltFactory(
        GraphicsService* _gfx,
        SharedData* _sharedData,
        InputState* _inputState,
        UiEltFactory* _uiEltFactory
    ) {
        gfx = _gfx;
        sharedData = _sharedData;
        inputState = _inputState;
        uiEltFactory = _uiEltFactory;
        rootUgenLock = &sharedData->rootUgenLock;
    }

    BaseElt* makeBangButton(BaseUgen* pBang, int x, int y) {
        ButtonElt* button = new ButtonElt(
            gfx,
            inputState,
            {20, 20, 50, 50},
            lightGray,
            gray
        );

        SharedData* pSharedData = sharedData;

        button->onLeftClick = [pSharedData = pSharedData, pBang = pBang](
            int x,
            int y
        ) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)pBang, 0 };
            pSharedData->toAudio.enqueue(message);
        };

        return button;
    }

    ContainerElt* makeAHRExpEnvControls(
        std::wstring prefix,
        AHRExpEnv* p_env,
        int x,
        int y
    ) {
        ContainerElt* container = new ContainerElt(
            gfx,
            // { x, y, 500, 160 },
            { x, y, 160, 160 },
            true
        );

        int padding = 10;
        x = padding;
        y = padding;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Attack",
                sampstoms(p_env->attackSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setAttack(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Hold",
                sampstoms(p_env->holdSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setHold(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Release",
                sampstoms(p_env->releaseSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setRelease(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        return container;
    }

    ContainerElt* makeAHRExpEnvVcaControls(
        std::wstring prefix,
        AHRExpEnvVca* p_env,
        int x,
        int y
    ) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 160, 140 },
            false
        );

        x = 0;
        y = 0;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Attack",
                sampstoms(p_env->attackSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setAttack(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Hold",
                sampstoms(p_env->holdSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setHold(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Release",
                sampstoms(p_env->releaseSamps),
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    p_env->setRelease(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        return container;
    }

    ContainerElt* makeAHRExpEnvVcaScaleControls(
        std::wstring prefix,
        AHRExpEnvVcaScale* pEnv,
        int x,
        int y
    ) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 300, 140 },
            false
        );

        x = 0;
        y = 0;
        int originalY = y;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
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

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
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

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
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

        // scale

        x += 200;
        y = originalY;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" Low",
                pEnv->ahrScaleData.low,
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    pEnv->setScaleLow(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeNumberAndLabel(
                prefix + L" High",
                pEnv->ahrScaleData.high,
                0,
                10000,
                x,
                y,
                [=](int newNumber) {
                    rootUgenLock->lock();
                    pEnv->setScaleHigh(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        return container;
    }
};