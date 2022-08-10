#pragma once

#include <cstdlib>
#include <iostream>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"

class DoubleBuffer {
public:
    double* data = nullptr;
    size_t size = 0;
    double min = 0.0;
    double max = 0.0;

    void init(size_t size, double min, double max) {
        this->size = size;
        this->min = min;
        this->max = max;
        data = new double[size];
    }

    void destroy() {
        delete[] data;
    }
};

class UnsignedBuffer {
public:
    unsigned* data = nullptr;
    size_t size = 0;
    unsigned min = 0;
    unsigned max = 0;

    void init(size_t size, unsigned min, unsigned max) {
        this->size = size;
        this->min = min;
        this->max = max;
        data = new unsigned[size];
    }

    void destroy() {
        delete[] data;
    }
};

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

        double* doubleArr = new double[w];
        double step = twoPi / w;
        double cur = 0.0;

        for (size_t i = 0; i < w; i++) {
            doubleArr[i] = sin(cur);
            cur += step;
        }

        set(doubleArr, w);

        delete[] doubleArr;
    }

    void set(double* wave, size_t size) {
        for (size_t i = 0; i < w; i++) {
            unsigned y = sampleToYPixel(wave[i]);
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
