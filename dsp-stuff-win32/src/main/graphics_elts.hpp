#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "src/main/bitmap.hpp"

enum GraphicsEltTag { G_RECT, G_TEXT, G_BITMAP };

struct GraphicsElt {
    GraphicsEltTag tag;
    int z = 0;
    D2D1_RECT_F rect;
    D2D1_COLOR_F color;
    const wchar_t* text = nullptr;
    Bitmap* bitmap = nullptr;
};

inline GraphicsElt makeRectGfxElt(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
    GraphicsElt elt;
    elt.tag = G_RECT;
    elt.z = z;
    elt.rect = rect;
    elt.color = color;
    return elt;
}

inline GraphicsElt makeTextGfxElt(const wchar_t* text, const D2D1_RECT_F& rect, int z=0) {
    GraphicsElt elt;
    elt.tag = G_TEXT;
    elt.z = z;
    elt.rect = rect;
    elt.text = text;
    return elt;
}

inline GraphicsElt makeBitmapGfxElt(Bitmap* bitmap, const D2D1_RECT_F& rect, int z=0) {
    GraphicsElt elt;
    elt.tag = G_BITMAP;
    elt.z = z;
    elt.rect = rect;
    elt.bitmap = bitmap;
    return elt;
}
