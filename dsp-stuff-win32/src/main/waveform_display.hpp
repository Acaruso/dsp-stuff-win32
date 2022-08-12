#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"

class WaveformDisplay {
public:
    GraphicsService* gfx = nullptr;
    Bitmap* bitmap = nullptr;
    std::vector<double> wave;
    D2D1_COLOR_F fgColor = black;
    D2D1_COLOR_F bgColor = white;
    unsigned w = 0;
    unsigned h = 0;
    unsigned midpoint = 0;

    void init(GraphicsService* gfx, unsigned w, unsigned h) {
        this->gfx = gfx;
        this->w = w;
        this->h = h;
        this->midpoint = h / 2;

        bitmap = gfx->makeBitmap(w, h);
        bitmap->fill(bgColor);
    }

    void setWave(std::vector<double>& wave) {
        this->wave = wave;
        
        bitmap->fill(bgColor);

        size_t step = (wave.size() > w) ? (wave.size() / w) : 1;

        for (size_t x = 0; x < wave.size() && x < w; x++) {
            unsigned y = sampleToYPixel(wave[x * step]);
            drawVerticalLine(x, midpoint, y);
        }
    }

    void draw(unsigned x, unsigned y) {
        D2D1_RECT_F rect = makeRectF(x, y, w, h);
        gfx->drawBitmap(bitmap, rect);
    }

    void draw(D2D1_RECT_F& rect) {
        gfx->drawBitmap(bitmap, rect);
    }

private:
    unsigned sampleToYPixel(double sample) {
        return h - (unsigned)(((sample * 0.5) + 0.5) * h);
    }

    void drawHorizontalLine(unsigned x1, unsigned x2, unsigned y) {
        unsigned biggerX = x1 >= x2 ? x1 : x2;
        unsigned smallerX = x1 < x2 ? x1 : x2;

        for (unsigned x = smallerX; x < biggerX; ++x) {
            bitmap->setPixel(x, y, fgColor);
        }
    }

    void drawVerticalLine(unsigned x, unsigned y1, unsigned y2) {
        unsigned biggerY = y1 >= y2 ? y1 : y2;
        unsigned smallerY = y1 < y2 ? y1 : y2;

        for (unsigned y = smallerY; y < biggerY; y++) {
            bitmap->setPixel(x, y, fgColor);
        }
    }
};
