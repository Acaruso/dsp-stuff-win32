#pragma once

#include <string>

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

    BaseElt* makeBangButton(BaseUgen* p_bang, int x, int y) {
        ButtonElt* button = new ButtonElt(
            gfx,
            inputState,
            {20, 20, 50, 50},
            lightGray,
            gray
        );

        button->onLeftClick = [=](
            int x,
            int y
        ) {
            ToAudioMessage message = { AM_TRIG, (uint64_t)p_bang, 0 };
            sharedData->toAudio.enqueue(message);
        };

        return button;
    }

    template <typename T>
    ContainerElt* makeAHREnvControls(
        std::wstring prefix,
        T* p_env,
        int x,
        int y
    ) {
        ContainerElt* container = new ContainerElt(
            gfx,
            { x, y, 160, 160 },
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

    template <typename T>
    ContainerElt* makeAHREnvScaleControls(
        std::wstring prefix,
        T* p_env,
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

        // scale

        x += 200;
        y = originalY;

        container->pushChild(
            uiEltFactory->makeFloatNumberAndLabel(
                prefix + L" Low",
                p_env->ahrScaleData.low,
                0.0f,
                10000.0f,
                5,
                1,
                x,
                y,
                [=](float newNumber) {
                    rootUgenLock->lock();
                    p_env->setScaleLow(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        y += 50;

        container->pushChild(
            uiEltFactory->makeFloatNumberAndLabel(
                prefix + L" High",
                p_env->ahrScaleData.high,
                0.0f,
                10000.0f,
                5,
                1,
                x,
                y,
                [=](float newNumber) {
                    rootUgenLock->lock();
                    p_env->setScaleHigh(newNumber);
                    rootUgenLock->unlock();
                }
            )
        );

        return container;
    }
};
