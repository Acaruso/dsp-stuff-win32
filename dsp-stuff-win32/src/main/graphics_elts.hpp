#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "src/main/bitmap.hpp"

enum GraphicsEltTag {
    G_RECT,
    G_TEXT,
    G_BITMAP,
    G_LINE
};

struct GraphicsElt {
    GraphicsEltTag tag;
    int z = 0;
    D2D1_RECT_F rect;
    bool outline = false;
    D2D1_COLOR_F color;
    const wchar_t* text = nullptr;
    Bitmap* bitmap = nullptr;
    D2D1_POINT_2F point0;
    D2D1_POINT_2F point1;
};

inline GraphicsElt makeRectGfxElt(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
    GraphicsElt elt;
    elt.tag = G_RECT;
    elt.z = z;
    elt.rect = rect;
    elt.color = color;
    return elt;
}

inline GraphicsElt makeTextGfxElt(const wchar_t* text, const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, int z=0) {
    GraphicsElt elt;
    elt.tag = G_TEXT;
    elt.z = z;
    elt.rect = rect;
    elt.text = text;
    elt.color = color;
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

inline GraphicsElt makeLineGfxElt(
    D2D1_POINT_2F point0,
    D2D1_POINT_2F point1,
    D2D1_COLOR_F& color,
    int z=0
) {
    GraphicsElt elt;
    elt.tag = G_LINE;
    elt.point0 = point0;
    elt.point1 = point1;
    elt.color = color;
    elt.z = z;
    return elt;
}
