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
#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/advanced/waveform_elt.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/button_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/ui_elts/ui_elt_util.hpp"
#include "src/main/util.hpp"
#include "src/main/waveform_display.hpp"
#include "src/shared/shared_data.hpp"
#include "src/shared/shared_util.hpp"

class App {
public:
    HWND window;
    GraphicsService gfx;
    SharedData sharedData;
    std::thread audioThread;
    WaveformDisplay waveformDisplay;
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

    BaseElt* uiRoot;

    HRESULT init(HWND window) {
        HRESULT hr;
        this->window = window;

        hr = gfx.init(window);

        audioThread = std::thread(&audioMain, &sharedData);

        D2D1_RECT_F waveformRect = makeRectF(20, 20, 1400, 100);
        waveformDisplay.init(&gfx, waveformRect, green);

        initUi();

        return hr;
    }

    void initUi() {
        uiRoot = new ContainerElt(&gfx, makeRectF(0, 0, windowWidth, windowHeight));

        int pad = 6;

        RectWH waveRect = {pad, pad, 800, 200};
        RectWH buttonRect = {waveRect.x + waveRect.w + pad, waveRect.y, 40, 40};
        RectWH waveContainerRect = {
            20,
            20,
            pad + waveRect.w + pad + buttonRect.w + pad,
            pad + waveRect.h + pad
        };

        BaseElt* waveContainer = new ContainerElt(&gfx, makeRectF(waveContainerRect), true);
        uiRoot->pushChild(waveContainer);

        BaseElt* waveformElt = new WaveformElt(&gfx, &inputState, &sharedData, makeRectF(waveRect));
        waveContainer->pushChild(waveformElt);

        ButtonElt* buttonElt = new ButtonElt(&gfx, &inputState, makeRectF(buttonRect), white, green);
        buttonElt->onLeftClick = [&](int x, int y) {
            sharedData.toAudio.enqueue("trig");
        };
        waveContainer->pushChild(buttonElt);
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
        inputState.isActiveWindow = (window == GetActiveWindow());
        handleTick(uiRoot);
        prevInputState = inputState;
        gfx.invalidateWindow();
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
