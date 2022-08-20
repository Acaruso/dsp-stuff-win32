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
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/ui_elts/basic/rect_elt.hpp"
#include "src/main/ui_elts/basic/text_elt.hpp"
#include "src/main/ui_elts_intrusive/rect_elt_intrusive.hpp"
#include "src/main/ui_elts_intrusive/ui_tree_node.hpp"
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
    WaveformElt* waveformElt;

    UiTreeNode* intrusiveRoot;

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

    // void initUi() {
    //     uiRoot = new ContainerElt(&gfx, makeRectF(20, 20, 1000, 1000));

    //     waveformElt = new WaveformElt(&gfx, makeRectF(0, 0, 800, 200));
    //     waveformElt->sharedData = &sharedData;

    //     uiRoot->pushChild(waveformElt);

    //     BaseElt* textElt = new TextElt(&gfx, makeRectF(0, 400, 800, 200), L"some text");

    //     uiRoot->pushChild(textElt);
    // }

    void initUi() {
        uiRoot = new ContainerElt(&gfx, makeRectF(0, 0, 0, 0));

        intrusiveRoot = new UiTreeNode;
        intrusiveRoot->gfx = &gfx;
        intrusiveRoot->rect = makeRectF(0, 0, 2000, 2000);

        RectEltIntrusive* rectI = new RectEltIntrusive(&gfx, makeRectF(20, 20, 20, 20));

        intrusiveRoot->pushChild(&rectI->uiTreeNode);
    }

    bool shouldHandleMessage(UINT message) {
        return (messageTypes.find(message) != messageTypes.end());
    }

    HRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        HRESULT hr = S_OK;

        if (message == WM_PAINT) {
            hr = onPaint();
        } else if (message == WM_LBUTTONDOWN) {
            onLeftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_RBUTTONDOWN) {
            onRightClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_MOUSEMOVE) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            onMouseMove(x, y);
            if (getKeyState(VK_LBUTTON)) {
                onLeftDrag(x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
            }
        } else if (message == WM_MOUSEWHEEL) {
            onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        } else if (message == WM_KEYDOWN) {
            onKeyDown(wParam, lParam);
        }

        return hr;
    }

    HRESULT onPaint() {
        HRESULT hr = S_OK;
        gfx.beginDraw();
        gfx.clear();

        // uiRoot->handleDraw();

        intrusiveRoot->handleDraw();

        gfx.render();
        hr = gfx.endDraw();
        return hr;
    }

    void tick() {
        if (window == GetActiveWindow()) {
            uiRoot->handleTick(inputState);
        }

        prevInputState = inputState;
        gfx.invalidateWindow();
    }

    void onKeyDown(WPARAM wParam, LPARAM lParam) {
        uiRoot->handleKeyDown(inputState, wParam);
    }

    void onLeftClick(int x, int y) {
        uiRoot->handleLeftClick(x, y);
    }

    void onLeftDrag(int x, int y, int xDelta, int yDelta) {
        uiRoot->handleLeftDrag(x, y, xDelta, yDelta);
    }

    void onRightClick(int x, int y) { }

    void onMouseMove(int x, int y) {
        inputState.mouseX = x;
        inputState.mouseY = y;
    }

    void onMouseWheel(int wheelDelta) {
        uiRoot->handleMouseWheel(inputState, wheelDelta);
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
