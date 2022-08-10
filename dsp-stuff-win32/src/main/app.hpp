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
#include "src/shared/shared_data.hpp"

class App {
public:
    HWND window;
    GraphicsService gfx;
    SharedData sharedData;
    std::thread audioThread;
    D2D1_RECT_F rect = D2D1::RectF(100, 100, 150, 150);
    Bitmap* bitmap;
    std::vector<UINT> messageTypes{
        WM_PAINT,
        WM_LBUTTONDOWN,
        WM_MOUSEMOVE
    };

    HRESULT init(HWND window) {
        HRESULT hr;
        this->window = window;
        hr = gfx.init(window);
        audioThread = std::thread(&audioMain, &sharedData);

        bitmap = gfx.createBitmap(512, 512);

        randomlyFillBitmap(bitmap);

        return hr;
    }

    void randomlyFillBitmap(Bitmap* bitmap) {
        double r = 0.0;

        for (int row = 0; row < bitmap->w; row++) {
            for (int col = 0; col < bitmap->h; col++) {
                r = getRand();
                if (r > 0.5) {
                    bitmap->setPixel(row, col, blue);
                }
            }
        }
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
        } else if (message == WM_MOUSEMOVE) {
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }

        return hr;
    }

    void tick() {
        if (getKeyState(VK_LEFT)) {
            rect = moveRect(rect, rect.left - 5, rect.top);
        }

        if (getKeyState(VK_RIGHT)) {
            rect = moveRect(rect, rect.left + 5, rect.top);
        }

        if (getKeyState(VK_UP)) {
            rect = moveRect(rect, rect.left, rect.top - 5);
        }

        if (getKeyState(VK_DOWN)) {
            rect = moveRect(rect, rect.left, rect.top + 5);
        }

        gfx.invalidateWindow();
    }

    HRESULT onPaint() {
        HRESULT hr = S_OK;

        gfx.beginDraw();

        gfx.clear();

        gfx.drawRect(rect, black);

        const wchar_t* text = L"Hello World test 123456 sdfsfdsdfsdfsdfsdf";
        D2D1_RECT_F textRect = D2D1::RectF(0, 0, 100, 100);
        gfx.drawText(text, textRect, 1);
        gfx.drawRect(textRect, blue);

        D2D1_RECT_F bitmapRect = D2D1::RectF(200, 200, 200 + 512, 200 + 512);
        gfx.drawBitmap(bitmap, bitmapRect, 10);

        gfx.render();

        hr = gfx.endDraw();

        return hr;
    }

    void onLeftClick(int x, int y) {
        rect = moveRect(rect, x, y);
        sharedData.toAudio.enqueue("trig");
    }

    void onMouseMove(int x, int y) {
    }

    void destroy() {
        gfx.destroy();
        sharedData.toAudio.enqueue("quit");
        audioThread.join();
    }
};
