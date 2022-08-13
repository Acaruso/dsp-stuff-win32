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
            onLeftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_RBUTTONDOWN) {
            onRightClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_MOUSEMOVE) {
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_KEYDOWN) {
            onKeyDown(wParam, lParam);
        }

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

        gfx.invalidateWindow();
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

    void onLeftClick(int x, int y) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onLeftClick(x, y);
        }
    }

    void onRightClick(int x, int y) {
        if (isInsideRect(x, y, waveformDisplay.rect)) {
            waveformDisplay.onRightClick(x, y);
        }
    }

    void onMouseMove(int x, int y) { }

    void onKeyDown(WPARAM wParam, LPARAM lParam) {
        if (wParam == VK_SPACE) {
            sharedData.toAudio.enqueue("trig");
        }
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
