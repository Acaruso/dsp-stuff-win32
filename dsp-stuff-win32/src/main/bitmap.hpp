#pragma once

#include <iostream>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "src/main/constants.hpp"
#include "src/main/util.hpp"

class Bitmap {
public:
    Bitmap(unsigned w, unsigned h, ID2D1HwndRenderTarget* renderTarget)
        : w(w), h(h), renderTarget(renderTarget)
    {
        HRESULT hr;

        D2D1_PIXEL_FORMAT pixelFormat = renderTarget->GetPixelFormat();
        if (pixelFormat.format != DXGI_FORMAT_B8G8R8A8_UNORM) {
            std::cout << "Bitmap: unsupported pixel format" << std::endl;
            return;
        }

        byteArr = new byte[(w * bytesPerPixel) * h];

        fill(white);

        hr = renderTarget->CreateBitmap(
            D2D1::SizeU(w, h),
            D2D1::BitmapProperties(pixelFormat),
            &d2dBitmap
        );
    }

    void setPixel(unsigned x, unsigned y, D2D1_COLOR_F color) {
        byte b = color.b * scale;
        byte g = color.g * scale;
        byte r = color.r * scale;
        byte a = color.a * scale;

        unsigned i = (y * (w * bytesPerPixel)) + (x * bytesPerPixel);

        byteArr[i]     = b;
        byteArr[i + 1] = g;
        byteArr[i + 2] = r;
        byteArr[i + 3] = a;
    }

    void fill(D2D1_COLOR_F color) {
        for (unsigned x = 0; x < h; x++) {
            for (unsigned y = 0; y < w; y++) {
                setPixel(x, y, color);
            }
        }
    }

    ~Bitmap() {
        delete[] byteArr;
        safeRelease(&d2dBitmap);
    }

    byte* byteArr = nullptr;
    unsigned w = 0;
    unsigned h = 0;
    unsigned bytesPerPixel = 4;
    byte scale = (1 << 8) - 1;
    ID2D1HwndRenderTarget* renderTarget = nullptr;
    ID2D1Bitmap* d2dBitmap = nullptr;
};
