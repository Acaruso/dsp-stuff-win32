#pragma once

#include <cstdio>
#include <memory>
#include <thread>
#include <unordered_set>
#include <vector>

#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include "src/audio/audio_main.hpp"
#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/ui_elts/composite/composites.hpp"
#include "src/main/ui_elts/ui_elt_util.hpp"
#include "src/main/util.hpp"
#include "src/shared/shared_data.hpp"

class App {
public:
    HWND window;
    GraphicsService gfx;
    SharedData sharedData;
    std::thread audioThread;
    std::unordered_set<UINT> messageTypes{
        WM_PAINT,
        WM_LBUTTONDOWN,
        WM_RBUTTONDOWN,
        WM_MOUSEMOVE,
        WM_MOUSEWHEEL,
        WM_KEYDOWN
    };
    InputState inputState;
    InputState prevInputState;
    CompositeFactory* compositeFactory = nullptr;
    BaseElt* uiRoot = nullptr;

    int yInc = 250;
    RectWH oscRect = { 20, 20, 900, 200 };

    HRESULT init(HWND window) {
        HRESULT hr;
        this->window = window;
        hr = gfx.init(window);
        audioThread = std::thread(&audioMain, &sharedData);
        compositeFactory = new CompositeFactory(&gfx, &inputState, &sharedData);
        uiRoot = new ContainerElt(&gfx, makeRectF(0, 0, windowWidth, windowHeight));
        return hr;
    }

    void initUi() {
        sharedData.rootUgenLock.lock();

        makeOscUgenAndUi(oscRect);
        oscRect.y += yInc;

        // button to add new ugen
        RectWH buttonRect = { 960, 20, 40, 40 };

        ButtonElt* button = new ButtonElt(&gfx, &inputState, makeRectF(buttonRect), lightGray, gray);

        button->onLeftClick = [&](int x, int y) {
            sharedData.rootUgenLock.lock();
            makeOscUgenAndUi(oscRect);
            oscRect.y += yInc;
            sharedData.rootUgenLock.unlock();
        };

        uiRoot->pushChild(button);

        sharedData.rootUgenLock.unlock();
    }

    void makeOscUgenAndUi(RectWH oscRect) {
        // create osc
        UgenManager* root = &sharedData.rootUgen;

        double freq = 120.0;

        UgenManager* pOsc = makeOscEnvFMUnisonRecorder(freq);

        int osc = root->addUgen(pOsc);

        root->connectOut(osc, 0, 0);

        // create osc ui elt
        uiRoot->pushChild(compositeFactory->makeTwoWavesAndButton(pOsc, oscRect));
    }

    bool shouldHandleMessage(UINT message) {
        return (messageTypes.find(message) != messageTypes.end());
    }

    HRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        HRESULT hr = S_OK;

        switch (message) {
            case WM_PAINT: {
                hr = onPaint();
                break;
            }
            case WM_LBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                handleLeftClick(uiRoot, x, y);
                break;
            }
            case WM_RBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                std::cout << "x: " << x << " y: " << y << std::endl;
                break;
            }
            case WM_MOUSEMOVE: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                inputState.mouseX = x;
                inputState.mouseY = y;
                if (getKeyState(VK_LBUTTON)) {
                    handleLeftDrag(uiRoot, x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                handleMouseWheel(uiRoot, inputState, wheelDelta);
                break;
            }
            case WM_KEYDOWN: {
                handleKeyDown(uiRoot, inputState, wParam);
                break;
            }
        }

        return hr;
    }

    HRESULT onPaint() {
        HRESULT hr = S_OK;
        gfx.beginDraw();
        gfx.clear();
        handleDraw(&gfx, uiRoot);
        gfx.render();
        hr = gfx.endDraw();
        return hr;
    }

    void tick() {
        ToMainMessage message;
        while (sharedData.toMain.try_dequeue(message)) {
            switch (message.type) {
                case TM_INIT_FINISHED:
                    initUi();
                    break;
                case TM_NO_MESSAGE:
                    break;
            }
        }

        inputState.isActiveWindow = (window == GetActiveWindow());
        handleTick(uiRoot);
        prevInputState = inputState;
        gfx.invalidateWindow();
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue(ToAudioMessage{AM_QUIT, 0, 0});
        audioThread.join();
    }
};
