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

        byteArr = new byte[w * h * 4];

        Color whiteColor = dColorToColor(white);
        fill(whiteColor);

        hr = renderTarget->CreateBitmap(
            D2D1::SizeU(w, h),
            D2D1::BitmapProperties(pixelFormat),
            &bitmap
        );
    }

    // color is defined in util.h -- should we move it somewhere else?
    void setPixel(unsigned x, unsigned y, Color color) {
        unsigned i = w * 4 * y + x * 4;

        byteArr[i]     = color.b;
        byteArr[i + 1] = color.g;
        byteArr[i + 2] = color.r;
        byteArr[i + 3] = color.a;
    }

    void fill(Color color) {
        for (unsigned row = 0; row < h; row++) {
            for (unsigned col = 0; col < w; col++) {
                setPixel(row, col, color);
            }
        }
    }

    ~Bitmap() {
        delete[] byteArr;
        safeRelease(&bitmap);
    }

private:
    byte* byteArr = nullptr;
    unsigned w = 0;
    unsigned h = 0;
    ID2D1HwndRenderTarget* renderTarget = nullptr;
    ID2D1Bitmap* bitmap = nullptr;
};
