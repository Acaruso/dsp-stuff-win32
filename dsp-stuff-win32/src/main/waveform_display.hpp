#pragma once

#include <cstdlib>
#include <iostream>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"

double pi = 3.14159265359;
double twoPi = pi * 2;

double* makeSineWave(size_t size) {
    double* arr = new double[size];

    double step = twoPi / size;
    double cur = 0.0;

    for (size_t i = 0; i < size; i++) {
        arr[i] = sin(cur);
        cur += step;
    }

    return arr;
}

class WaveformDisplay {
public:
    GraphicsService* gfx = nullptr;
    Bitmap* bitmap = nullptr;
    unsigned w = 0;
    unsigned h = 0;

    void init(GraphicsService* gfx, unsigned w, unsigned h) {
        this->gfx = gfx;
        this->w = w;
        this->h = h;

        bitmap = gfx->makeBitmap(w, h);
        bitmap->fill(blue);

        // size_t doubleArrSize = 100000;
        // double* doubleArr = makeSineWave(doubleArrSize);
        // setWave(doubleArr, doubleArrSize);
        // delete[] doubleArr;
    }

    void setWave(double* wave, size_t size) {
        size_t step = (size > w) ? (size / w) : 1;

        for (size_t i = 0; i < size && i < w; i++) {
            unsigned y = sampleToYPixel(wave[i * step]);
            drawLine(i, y);
        }
    }

    void draw(D2D1_RECT_F rect) {
        gfx->drawBitmap(bitmap, rect);
    }

private:
    unsigned sampleToYPixel(double sample) {
        return h - (unsigned)(((sample * 0.5) + 0.5) * h);
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
