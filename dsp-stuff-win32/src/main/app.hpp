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
#include "src/main/ui_elts/base_elt.hpp"
#include "src/main/ui_elts/container_elt.hpp"
#include "src/main/ui_elts/rect_elt.hpp"
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

        uiRoot = new ContainerElt(&gfx, makeRectF(0, 0, 2000, 2000));
        uiRoot->pushChild(new RectElt(&gfx, makeRectF(0, 0, 20, 20)));
        BaseElt* child = new ContainerElt(&gfx, makeRectF(100, 100, 200, 200));
        child->pushChild(new RectElt(&gfx, makeRectF(0, 0, 20, 20)));
        uiRoot->pushChild(child);

        return hr;
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

        if (sharedData.envOn) {
            waveformDisplay.setWave(sharedData.sampleBuffer);
        }
        // waveformDisplay.draw();

        uiRoot->draw();

        gfx.render();
        hr = gfx.endDraw();
        return hr;
    }

    void tick() {
        if (window == GetActiveWindow()) {
            if (getKeyState(VK_UP)) {
                waveformDisplay.zoom(20);
            }

            if (getKeyState(VK_DOWN)) {
                waveformDisplay.zoom(-20);
            }

            if (getKeyState(VK_LEFT)) {
                waveformDisplay.scroll(-10);
            }

            if (getKeyState(VK_RIGHT)) {
                waveformDisplay.scroll(10);
            }
        }

        prevInputState = inputState;

        gfx.invalidateWindow();
    }

    void onKeyDown(WPARAM wParam, LPARAM lParam) {
        if (wParam == VK_SPACE) {
            sharedData.toAudio.enqueue("trig");
        } else if (wParam == int('Z')) {
            waveformDisplay.zoomToSelection();
        }
    }

    void onLeftClick(int x, int y) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onLeftClick(x, y);
        }
    }

    void onLeftDrag(int x, int y, int xDelta, int yDelta) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onDrag(x, y, xDelta, yDelta);
        }
    }

    void onRightClick(int x, int y) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onRightClick(x, y);
        }
    }

    void onMouseMove(int x, int y) {
        inputState.mouseX = x;
        inputState.mouseY = y;
    }

    void onMouseWheel(int wheelDelta) {
        if (isInsideRect(inputState.mouseX, inputState.mouseY, waveformDisplay.rect)) {
            if (wheelDelta < 0) {
                waveformDisplay.zoom(-40);
            } else {
                waveformDisplay.zoom(40);
            }
        }
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
