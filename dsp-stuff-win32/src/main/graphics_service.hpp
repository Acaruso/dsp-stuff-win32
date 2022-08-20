#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <windowsx.h>
#include <winerror.h>
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

    Bitmap* makeBitmap(unsigned w, unsigned h) {
        return new Bitmap(renderTarget, w, h);
    }

    void drawRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
        D2D1_RECT_F offsetRect = makeOffsetRect(rect, xOffset, yOffset);
        GraphicsElt elt = makeRectGfxElt(offsetRect, color, z);
        drawQueue.push_back(elt);
    }

    void outlineRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
        D2D1_RECT_F offsetRect = makeOffsetRect(rect, xOffset, yOffset);
        GraphicsElt elt = makeRectGfxElt(offsetRect, color, z);
        elt.outline = true;
        drawQueue.push_back(elt);
    }

    void drawText(const wchar_t* text, const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
        D2D1_RECT_F offsetRect = makeOffsetRect(rect, xOffset, yOffset);
        GraphicsElt elt = makeTextGfxElt(text, offsetRect, color, z);
        drawQueue.push_back(elt);
    }

    void drawText(const wchar_t* text, const D2D1_RECT_F& rect, int z=0) {
        D2D1_RECT_F offsetRect = makeOffsetRect(rect, xOffset, yOffset);
        GraphicsElt elt = makeTextGfxElt(text, offsetRect, black, z);
        drawQueue.push_back(elt);
    }

    void drawBitmap(Bitmap* bitmap, D2D1_RECT_F& rect, int z=0) {
        D2D1_RECT_F offsetRect = makeOffsetRect(rect, xOffset, yOffset);
        GraphicsElt elt = makeBitmapGfxElt(bitmap, offsetRect, z);
        drawQueue.push_back(elt);
    }

    void pushOffset(D2D1_RECT_F offset) {
        offsets.push_back(offset);
        xOffset += offset.left;
        yOffset += offset.top;
    }

    void popOffset() {
        if (offsets.size() == 0) {
            return;
        }

        D2D1_RECT_F offset = offsets.back();
        xOffset -= offset.left;
        yOffset -= offset.top;
        offsets.pop_back();
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

private:
    HWND window;
    PAINTSTRUCT ps;
    ID2D1Factory* factory = nullptr;
    ID2D1HwndRenderTarget* renderTarget = nullptr;
    ID2D1SolidColorBrush* blackBrush = nullptr;
    IDWriteFactory* writeFactory = nullptr;
    IDWriteTextFormat* textFormat = nullptr;
    std::vector<GraphicsElt> drawQueue;
    std::vector<D2D1_RECT_F> offsets;
    int xOffset = 0;
    int yOffset = 0;

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

        D2D1_PIXEL_FORMAT pixelFormat = renderTarget->GetPixelFormat();
        std::cout << pixelFormatToString(pixelFormat) << std::endl;

        hr = renderTarget->CreateSolidColorBrush(black, &blackBrush);
        if (FAILED(hr)) {
            return hr;
        }

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
            nullptr,                          // pointer to font collection object (?)
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

    HRESULT _drawRect(const GraphicsElt& elt) {
        ID2D1SolidColorBrush* brush = nullptr;

        // does this ever actually fail?
        HRESULT hr = renderTarget->CreateSolidColorBrush(elt.color, &brush);
        if (FAILED(hr)) {
            return hr;
        }

        if (elt.outline) {
            D2D1_RECT_F newRect = elt.rect;
            newRect.left   += 0.5;
            newRect.top    += 0.5;
            newRect.right  += 0.5;
            newRect.bottom += 0.5;
            renderTarget->DrawRectangle(newRect, brush);
        } else {
            D2D1_RECT_F newRect = elt.rect;
            newRect.right  += 1;
            newRect.bottom += 1;
            renderTarget->FillRectangle(newRect, brush);
        }

        safeRelease(&brush);

        return hr;
    }

    HRESULT _drawText(const GraphicsElt& elt) {
        HRESULT hr = S_OK;

        ID2D1SolidColorBrush* brush = nullptr;

        hr = renderTarget->CreateSolidColorBrush(elt.color, &brush);
        if (FAILED(hr)) {
            return hr;
        }

        renderTarget->DrawTextW(
            elt.text,
            (UINT32)wcslen(elt.text),
            textFormat,
            elt.rect,
            brush
        );

        safeRelease(&brush);

        return hr;
    }

    void _drawBitmap(const GraphicsElt& elt) {
        Bitmap* bitmap = elt.bitmap;

        if (bitmap->modified) {
            D2D1_RECT_U tempRect = D2D1::RectU(0, 0, bitmap->w, bitmap->h);
            
            bitmap->d2dBitmap->CopyFromMemory(
                &tempRect,
                bitmap->byteArr, 
                bitmap->w * bitmap->bytesPerPixel
            );
        }

        bitmap->modified = false;
        
        renderTarget->DrawBitmap(bitmap->d2dBitmap, elt.rect, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    static bool drawQueueCompare(const GraphicsElt& a, const GraphicsElt& b) {
        return a.z > b.z;
    }

    void drawGraphicsElt(const GraphicsElt& elt) {
        if (elt.tag == G_RECT) {
            _drawRect(elt);
        } else if (elt.tag == G_TEXT) {
            _drawText(elt);
        } else if (elt.tag == G_BITMAP) {
            _drawBitmap(elt);
        }
    }

    void releaseGraphicsResources() {
        safeRelease(&renderTarget);
        safeRelease(&blackBrush);
    }
};
