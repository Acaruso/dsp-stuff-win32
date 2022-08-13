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

    unsigned cursor = 0;
    unsigned selectEnd = 0;
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

    void zoom(int delta_) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        int delta = delta_ * step;

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

    void scroll(int delta_) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        int delta = delta_ * step;

        if (inBounds(wave, windowBegin + delta) && inBounds(wave, windowEnd + delta)) {
            windowBegin += delta;
            windowEnd += delta;
            waveToPixels();
        }
    }

    void zoomToSelection() {
        if (selected) {
            unsigned s1 = mapPixelToSample(cursor);
            unsigned s2 = mapPixelToSample(selectEnd);
            unsigned smallerSample = s1 < s2 ? s1 : s2;
            unsigned biggerSample = s1 >= s2 ? s1 : s2;
            windowBegin = smallerSample;
            windowEnd = biggerSample;
            selected = false;
            waveToPixels();
        }
    }

    void onLeftClick(int x, int y) {
        cursor = x - rect.left;
        selected = false;
        waveToPixels();
    }

    void onDrag(int x, int y, int xDelta, int yDelta) {
        selectEnd = x - rect.left;
        selected = true;
        waveToPixels();
    }

    void onRightClick(int x, int y) { }

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
            unsigned sampleIdx = mapPixelToSample(pixelIdx);
            sample = inBounds(wave, sampleIdx) ? wave[sampleIdx] : 0.0;

            yPixel = sampleToYPixel(sample);

            if (selected && isInSelection(pixelIdx, cursor, selectEnd)) {
                D2D1_COLOR_F invertedBgColor = makeInvertedColor(bgColor);
                D2D1_COLOR_F invertedFgColor = makeInvertedColor(fgColor);
                drawVerticalLine(pixelIdx, 0, h, invertedBgColor);
                drawVerticalLine(pixelIdx, midpoint, yPixel, invertedFgColor);
            } else {
                drawVerticalLine(pixelIdx, midpoint, yPixel, fgColor);
            }
        }

        drawVerticalLine(cursor, 0, h, fgColor);

        if (selected) {
            drawVerticalLine(selectEnd, 0, h, fgColor);
        }
    }

    unsigned mapPixelToSample(unsigned pixelIdx) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        return windowBegin + (pixelIdx * step);
    }

    unsigned mapSampleToPixel(unsigned sampleIdx) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = (double)w / windowSize;
        return windowBegin + (sampleIdx * step);
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

    bool isInSelection(unsigned x, unsigned s1, unsigned s2) {
        unsigned biggerSelect = s1 >= s2 ? s1 : s2;
        unsigned smallerSelect = s1 < s2 ? s1 : s2;
        return (x < biggerSelect && x >= smallerSelect);
    }
};
