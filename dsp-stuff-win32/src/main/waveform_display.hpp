#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/shared/shared_util.hpp"

class WaveformDisplay {
public:
    GraphicsService* gfx = nullptr;
    Bitmap* bitmap = nullptr;
    std::vector<double> wave;
    D2D1_COLOR_F fgColor = black;
    D2D1_COLOR_F bgColor = white;
    D2D1_RECT_F rect;
    unsigned w = 0;
    unsigned h = 0;
    unsigned midpoint = 0;
    unsigned windowBegin = 0;
    unsigned windowEnd = 0;

    unsigned selectLeft = 0;
    unsigned selectRight = 0;
    bool selected = false;

    void init(GraphicsService* gfx, D2D1_RECT_F& rect) {
        this->gfx = gfx;
        this->rect = rect;
        this->w = rect.right - rect.left;
        this->h = rect.bottom - rect.top;
        this->midpoint = h / 2;

        bitmap = gfx->makeBitmap(w, h);
        bitmap->fill(bgColor);
    }

    void init(GraphicsService* gfx, D2D1_RECT_F& rect, D2D1_COLOR_F bgColor) {
        this->gfx = gfx;
        this->rect = rect;
        this->w = rect.right - rect.left;
        this->h = rect.bottom - rect.top;
        this->midpoint = h / 2;
        this->bgColor = bgColor;

        bitmap = gfx->makeBitmap(w, h);
        bitmap->fill(bgColor);
    }

    void setWave(std::vector<double>& wave) {
        this->wave = wave;
        windowBegin = 0;
        windowEnd = wave.size();
        waveToPixels();
    }

    void zoomIn(unsigned delta) {
        if (inBounds(wave, windowBegin + delta)) {
            windowBegin += delta;
        }

        if (inBounds(wave, windowEnd - delta)) {
            windowEnd -= delta;
        }

        if (inBounds(wave, windowBegin + delta) || inBounds(wave, windowEnd - delta)) {
            waveToPixels();
        }
    }

    void zoomOut(unsigned delta) {
        if (inBounds(wave, windowBegin - delta)) {
            windowBegin -= delta;
        }

        if (inBounds(wave, windowEnd + delta)) {
            windowEnd += delta;
        }
        
        if (inBounds(wave, windowBegin - delta) || inBounds(wave, windowEnd + delta)) {
            waveToPixels();
        }
    }

    void scrollLeft(unsigned delta) {
        if (inBounds(wave, windowBegin - delta) && inBounds(wave, windowEnd - delta)) {
            windowBegin -= delta;
            windowEnd -= delta;
            waveToPixels();
        }
    }

    void scrollRight(unsigned delta) {
        if (inBounds(wave, windowBegin + delta) && inBounds(wave, windowEnd + delta)) {
            windowBegin += delta;
            windowEnd += delta;
            waveToPixels();
        }
    }

    void onLeftClick(int x, int y) {
        selectLeft = x - rect.left;
        selected = true;
        waveToPixels();
    }

    void onRightClick(int x, int y) {
        selectRight = x - rect.left;
        selected = true;
        waveToPixels();
    }

    void draw() {
        gfx->drawBitmap(bitmap, rect);
    }

private:
    void waveToPixels() {
        bitmap->fill(bgColor);

        drawHorizontalLine(0, w, midpoint, fgColor);

        if (wave.size() == 0) {
            return;
        }

        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        double sample = 0.0;
        unsigned yPixel = 0;

        for (size_t pixelIdx = 0; pixelIdx < w; pixelIdx++) {
            sample = getWaveSample(pixelIdx, step);
            yPixel = sampleToYPixel(sample);
            drawVerticalLine(pixelIdx, midpoint, yPixel, fgColor);
        }

        if (selected) {
            drawVerticalLine(selectLeft, 0, h, fgColor);
            drawVerticalLine(selectRight, 0, h, fgColor);
        }
    }

    double getWaveSample(unsigned pixelIdx, double step) {
        unsigned waveIdx = windowBegin + (pixelIdx * step);
        return inBounds(wave, waveIdx) ? wave[waveIdx] : 0.0;
    }

    unsigned sampleToYPixel(double sample) {
        return h - (unsigned)(((sample * 0.5) + 0.5) * h);
    }

    void drawHorizontalLine(unsigned x1, unsigned x2, unsigned y, D2D1_COLOR_F& color) {
        unsigned biggerX = x1 >= x2 ? x1 : x2;
        unsigned smallerX = x1 < x2 ? x1 : x2;

        for (unsigned x = smallerX; x < biggerX; ++x) {
            bitmap->setPixel(x, y, color);
        }
    }

    void drawVerticalLine(unsigned x, unsigned y1, unsigned y2, D2D1_COLOR_F& color) {
        unsigned biggerY = y1 >= y2 ? y1 : y2;
        unsigned smallerY = y1 < y2 ? y1 : y2;

        for (unsigned y = smallerY; y < biggerY; y++) {
            bitmap->setPixel(x, y, color);
        }
    }
};
