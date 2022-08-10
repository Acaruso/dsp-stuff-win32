#pragma once

#include <cstdlib>
#include <iostream>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"

class WaveformDisplay {
public:
    GraphicsService* gfx = nullptr;
    Bitmap* bitmap = nullptr;
    unsigned w = 0;
    unsigned h = 0;

    double pi = 3.14159265359;
    double twoPi = pi * 2;

    void init(GraphicsService* gfx, unsigned w, unsigned h) {
        this->gfx = gfx;
        this->w = w;
        this->h = h;

        bitmap = gfx->makeBitmap(w, h);

        double step = twoPi / w;
        double cur = 0.0;

        double* doubleArr = new double[w];

        for (unsigned i = 0; i < w; i++) {
            doubleArr[i] = sin(cur);
            cur += step;
        }

        unsigned* unsignedArr = new unsigned[w];

        for (unsigned i = 0; i < w; i++) {
            double doubleElt = doubleArr[i];
            double doubleElt01 = (doubleElt * 0.5) + 0.5;
            unsignedArr[i] = (unsigned)(doubleElt01 * h);
        }

        for (unsigned i = 0; i < w; i++) {
            unsigned y = unsignedArr[i];
            drawLine(i, y);
        }

        delete[] doubleArr;
        delete[] unsignedArr;
    }

    void drawLine(unsigned x, unsigned y) {
        unsigned midpoint = h / 2;

        if (y >= midpoint) {
            for (unsigned i = midpoint; i < y; i++) {
                bitmap->setPixel(x, i, black);
            }
        } else if (y < midpoint) {
            for (unsigned i = midpoint; i > y; i--) {
                bitmap->setPixel(x, i, black);
            }
        }
    }
};
