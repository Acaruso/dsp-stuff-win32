#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_elts.hpp"
#include "src/shared/shared_util.hpp"

class GraphicsService {
public:
    HRESULT init(HWND window) {
        HRESULT hr;

        this->window = window;

        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        if (FAILED(hr)) {
            return hr;
        }

        hr = createGraphicsResources();
        if (FAILED(hr)) {
            return hr;
        }

        hr = createFontResources();
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    void beginDraw() {
        BeginPaint(window, &ps);
        renderTarget->BeginDraw();
    }

    HRESULT endDraw() {
        HRESULT hr = renderTarget->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            std::cout << "recreating graphics resources" << std::endl;
            releaseGraphicsResources();
            createGraphicsResources();
            hr = S_OK;
        }

        EndPaint(window, &ps);

        return hr;
    }

    void clear() {
        renderTarget->Clear(white);
    }

    void drawRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
        GraphicsElt elt = makeRect(rect, color, z);
        drawQueue.push_back(elt);
    }

    void drawText(const wchar_t* text, const D2D1_RECT_F& rect, int z=0) {
        GraphicsElt elt = makeText(text, rect, z);
        drawQueue.push_back(elt);
    }

    void render() {
        std::sort(drawQueue.begin(), drawQueue.end(), drawQueueCompare);
        while (!drawQueue.empty()) {
            drawGraphicsElt(drawQueue.back());
            drawQueue.pop_back();
        }
    }

    void invalidateWindow() {
        InvalidateRect(window, nullptr, FALSE);
    }

    void destroy() {
        releaseGraphicsResources();
        safeRelease(&factory);
        safeRelease(&writeFactory);
        safeRelease(&textFormat);
    }

    void drawBitmap(D2D1_RECT_F rect) {
        _drawBitmap(bitmapMemory, bitmap, rect);
    }

    // Bitmap& createBitmap(unsigned w, unsigned h) {
    //     Bitmap newBitmap(w, h, renderTarget);
    //     bitmaps.push_back(newBitmap);
    //     return bitmaps.back();
    // }

    Bitmap* createBitmap(unsigned w, unsigned h) {
        return new Bitmap(w, h, renderTarget);
    }

    void drawBitmap(Bitmap& bitmap, D2D1_RECT_F rect) {
        D2D1_RECT_U tempRect = D2D1::RectU(0, 0, bitmap.w, bitmap.h);

        bitmap.d2dBitmap->CopyFromMemory(
            &tempRect,
            bitmap.byteArr, 
            bitmap.w * bitmap.bytesPerPixel
        );
        
        renderTarget->DrawBitmap(bitmap.d2dBitmap, rect, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

private:
    HWND window;
    PAINTSTRUCT ps;
    ID2D1Factory* factory = nullptr;
    ID2D1HwndRenderTarget* renderTarget = nullptr;
    ID2D1SolidColorBrush* blackBrush = nullptr;
    IDWriteFactory* writeFactory = nullptr;
    IDWriteTextFormat* textFormat = nullptr;
    std::vector<GraphicsElt> drawQueue;

    byte* bitmapMemory = nullptr;
    unsigned bitmapW = 512;
    unsigned bitmapH = 512;
    ID2D1Bitmap* bitmap = nullptr;

    std::vector<Bitmap> bitmaps;

    HRESULT createGraphicsResources() {
        HRESULT hr;

        RECT windowClientRect;
        GetClientRect(window, &windowClientRect);

        D2D1_SIZE_U windowClientRectSize = D2D1::SizeU(windowClientRect.right, windowClientRect.bottom);

        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(window, windowClientRectSize),
            &renderTarget
        );
        if (FAILED(hr)) {
            return hr;
        }

        hr = renderTarget->CreateSolidColorBrush(black, &blackBrush);
        if (FAILED(hr)) {
            return hr;
        }

        bitmapMemory = new byte[bitmapW * bitmapH * 4];

        D2D1_PIXEL_FORMAT pixelFormat = renderTarget->GetPixelFormat();
        std::cout << pixelFormatToString(pixelFormat) << std::endl;

        hr = renderTarget->CreateBitmap(
            D2D1::SizeU(bitmapW, bitmapH),
            D2D1::BitmapProperties(pixelFormat),
            &bitmap
        );
        if (FAILED(hr)) {
            return hr;
        }

        randomlyFillBitmapMemory();

        return hr;
    }

    HRESULT createFontResources() {
        HRESULT hr;

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&writeFactory)
        );
        if (FAILED(hr)) {
            return hr;
        }

        hr = writeFactory->CreateTextFormat(
            L"Consolas",                      // font family name
            NULL,                             // pointer to font collection object (?)
            DWRITE_FONT_WEIGHT_NORMAL,        // font weight
            DWRITE_FONT_STYLE_NORMAL,         // font style
            DWRITE_FONT_STRETCH_NORMAL,       // font stretch
            11.0f * 96.0f/72.0f,              // logical size of font in DIPs
            L"en-US",                         // locale name
            &textFormat                       // output
        );
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    HRESULT _drawRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color) {
        ID2D1SolidColorBrush* newBrush = NULL;

        // does this ever actually fail?
        HRESULT hr = renderTarget->CreateSolidColorBrush(color, &newBrush);
        if (FAILED(hr)) {
            return hr;
        }

        renderTarget->FillRectangle(rect, newBrush);

        safeRelease(&newBrush);

        return hr;
    }

    void _drawText(const wchar_t* text, const D2D1_RECT_F& layoutRect) {
        renderTarget->DrawText(
            text,
            wcslen(text),
            textFormat,
            layoutRect,
            blackBrush
        );
    }

    // bitmap stuff ///////////////////////////////////////

    void _drawPixel(int x, int y, Color color) {
        int i = (int)(bitmapW * 4 * y + x * 4);

        bitmapMemory[i]     = color.b;
        bitmapMemory[i + 1] = color.g;
        bitmapMemory[i + 2] = color.r;
        bitmapMemory[i + 3] = color.a;
    }

    void _drawBitmap(byte* bitmapMemory, ID2D1Bitmap* bitmap, D2D1_RECT_F rect) {
        D2D1_RECT_U rect_ = D2D1::RectU(0, 0, 512, 512);
        bitmap->CopyFromMemory(
            &rect_,
            bitmapMemory, 
            bitmapW * 4
        );
        renderTarget->DrawBitmap(bitmap, rect, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    // double getRand() {
    //     return rand() / (RAND_MAX + 1.0);
    // }

    void randomlyFillBitmapMemory() {
        Color bgColor = dColorToColor(white);
        Color fillColor = dColorToColor(black);
        double r = 0.0;

        for (int row = 0; row < bitmapW; row++) {
            for (int col = 0; col < bitmapH; col++) {
                _drawPixel(row, col, bgColor);
            }
        }

        for (int row = 0; row < bitmapW; row++) {
            for (int col = 0; col < bitmapH; col++) {
                r = getRand();
                if (r > 0.5) {
                    _drawPixel(row, col, fillColor);
                }
            }
        }
    }

    // end bitmap stuff ///////////////////////////////////

    static bool drawQueueCompare(const GraphicsElt& a, const GraphicsElt& b) {
        return a.z > b.z;
    }

    void drawGraphicsElt(const GraphicsElt& elt) {
        if (elt.tag == G_RECT) {
            _drawRect(elt.rect, elt.color);
        } else if (elt.tag == G_TEXT) {
            _drawText(elt.text, elt.rect);
        }
    }

    void releaseGraphicsResources() {
        safeRelease(&renderTarget);
        safeRelease(&blackBrush);
        safeRelease(&bitmap);
    }
};
