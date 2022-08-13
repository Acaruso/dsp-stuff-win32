#pragma once

#include <cstdio>
#include <thread>
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
    std::vector<UINT> messageTypes{
        WM_PAINT,
        WM_LBUTTONDOWN,
        WM_RBUTTONDOWN,
        WM_MOUSEMOVE,
        WM_KEYDOWN
    };
    InputState inputState;
    InputState prevInputState;

    HRESULT init(HWND window) {
        HRESULT hr;
        this->window = window;
        hr = gfx.init(window);
        audioThread = std::thread(&audioMain, &sharedData);
        D2D1_RECT_F waveformRect = makeRectF(20, 20, 1400, 100);
        waveformDisplay.init(&gfx, waveformRect, blue);
        return hr;
    }

    bool shouldHandleMessage(UINT message) {
        for (auto elt : messageTypes) {
            if (message == elt) {
                return true;
            }
        }
        return false;
    }

    HRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        HRESULT hr = S_OK;

        if (message == WM_PAINT) {
            hr = onPaint();
        } else if (message == WM_LBUTTONDOWN) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            // if (DragDetect(window, POINT{x, y})) {
            //     onLeftDrag(x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
            // } else {
            //     onLeftClick(x, y);
            // }

            onLeftClick(x, y);
            
        } else if (message == WM_RBUTTONDOWN) {
            onRightClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_MOUSEMOVE) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            onMouseMove(x, y);

            // if (DragDetect(window, POINT{x, y})) {
            //     if (getKeyState(VK_LBUTTON)) {
            //         onLeftDrag(x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
            //     }
            // }

            if (getKeyState(VK_LBUTTON)) {
                onLeftDrag(x, y, x - prevInputState.mouseX, y - prevInputState.mouseY);
            }
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
        waveformDisplay.draw();

        gfx.render();
        hr = gfx.endDraw();
        return hr;
    }

    void tick() {
        if (getKeyState(VK_UP)) {
            waveformDisplay.zoomIn(200);
        }

        if (getKeyState(VK_DOWN)) {
            waveformDisplay.zoomOut(200);
        }

        if (getKeyState(VK_LEFT)) {
            waveformDisplay.scrollLeft(100);
        }

        if (getKeyState(VK_RIGHT)) {
            waveformDisplay.scrollRight(100);
        }

        prevInputState = inputState;

        gfx.invalidateWindow();
    }

    void onKeyDown(WPARAM wParam, LPARAM lParam) {
        if (wParam == VK_SPACE) {
            sharedData.toAudio.enqueue("trig");
        }
    }

    void onLeftClick(int x, int y) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onLeftClick(x, y);
        }
    }

    void onLeftDrag(int x, int y, int xDelta, int yDelta) {
        std::cout << "xDelta: " << xDelta << " yDelta: " << yDelta << std::endl;
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

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
