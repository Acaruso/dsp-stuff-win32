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
#include "src/audio/ugens/sink.hpp"
#include "src/audio/ugens/sum.hpp"
#include "src/audio/ugens/ugen_ctx.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/ui_elt_util.hpp"
#include "src/main/ui.hpp"
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
    Ui ui;
    BaseElt* uiRoot = nullptr;

    HRESULT init(HWND _window) {
        window = _window;
        HRESULT hr = gfx.init(window);

        UgenManager* root = &sharedData.rootUgen;
        UgenCtx* ugenCtx = root->ugenCtx;

        int outSum = root->addUgen("outSum", new Sum(ugenCtx, 0));
        int outSink = root->addUgen("outSink", new Sink(ugenCtx));

        root->connect(outSum, 0, outSink, 0);

        audioThread = std::thread(&audioMain, &sharedData);

        ui.init(&gfx, &sharedData, &inputState);
        uiRoot = ui.uiRoot;

        return hr;
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
                handleLeftClick(
                    uiRoot, 
                    GET_X_LPARAM(lParam), 
                    GET_Y_LPARAM(lParam)
                );
                break;
            }
            case WM_RBUTTONDOWN: {
                break;
            }
            case WM_MOUSEMOVE: {
                if (getKeyState(VK_LBUTTON)) {
                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);
                    inputState.mouseX = x;
                    inputState.mouseY = y;
                    handleLeftDrag(uiRoot, x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                handleMouseWheel(
                    uiRoot, 
                    inputState, 
                    GET_WHEEL_DELTA_WPARAM(wParam)
                );
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
                    ui.initUi();
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
