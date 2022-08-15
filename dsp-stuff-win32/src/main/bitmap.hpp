#pragma once

#include <iostream>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "src/main/constants.hpp"
#include "src/main/util.hpp"

class Bitmap {
public:
    Bitmap(ID2D1HwndRenderTarget* renderTarget, int w, int h)
        : w(w), h(h), renderTarget(renderTarget)
    {
        HRESULT hr;

        D2D1_PIXEL_FORMAT pixelFormat = renderTarget->GetPixelFormat();
        if (pixelFormat.format != DXGI_FORMAT_B8G8R8A8_UNORM) {
            std::cout << "Bitmap: unsupported pixel format" << std::endl;
            return;
        }

        hr = renderTarget->CreateBitmap(
            D2D1::SizeU(w, h),
            D2D1::BitmapProperties(pixelFormat),
            &d2dBitmap
        );

        byteArrSize = (w * bytesPerPixel) * h;
        byteArr = new byte[byteArrSize];

        clear();
    }

    void setPixel(int x, int y, D2D1_COLOR_F color) {
        static byte scale = (1 << 8) - 1;

        modified = true;

        byte b = (byte)(color.b * scale);
        byte g = (byte)(color.g * scale);
        byte r = (byte)(color.r * scale);
        byte a = (byte)(color.a * scale);

        int i = (y * (w * bytesPerPixel)) + (x * bytesPerPixel);

        byteArr[i]     = b;
        byteArr[i + 1] = g;
        byteArr[i + 2] = r;
        byteArr[i + 3] = a;
    }

    void fill(D2D1_COLOR_F color) {
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                setPixel(col, row, color);
            }
        }
    }

    void clear() {
        fill(white);
    }

    ~Bitmap() {
        delete[] byteArr;
        safeRelease(&d2dBitmap);
    }

    ID2D1HwndRenderTarget* renderTarget = nullptr;
    ID2D1Bitmap* d2dBitmap = nullptr;
    int bytesPerPixel = 4;    // assumes pixel format is DXGI_FORMAT_B8G8R8A8_UNORM
    size_t byteArrSize = 0;
    byte* byteArr = nullptr;
    int w = 0;
    int h = 0;
    bool modified = true;
};
