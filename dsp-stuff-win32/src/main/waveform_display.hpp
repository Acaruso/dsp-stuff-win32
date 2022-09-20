#pragma once

#include <iostream>
#include <vector>

#include "src/main/bitmap.hpp"
#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/shared/audio_buffer.hpp"
#include "src/shared/shared_util.hpp"

class WaveformDisplay {
public:
    GraphicsService* gfx = nullptr;
    Bitmap* bitmap = nullptr;
    AudioBuffer wave;
    D2D1_COLOR_F fgColor;
    D2D1_COLOR_F bgColor;
    D2D1_COLOR_F invertedBgColor;
    D2D1_COLOR_F invertedFgColor;
    D2D1_RECT_F rect;
    int w = 0;
    int h = 0;
    int midpoint = 0;
    int windowBegin = 0;
    int windowEnd = 0;
    int cursor = 0;           // sample location
    int selectEnd = 0;        // sample location
    bool selected = false;

    void init(
        GraphicsService* gfx,
        D2D1_RECT_F& rect
    ) {
        this->gfx = gfx;
        setRect(rect);
        setFgColor(black);
        setBgColor(white);
        bitmap = gfx->makeBitmap(w, h);
        updateBitmap();
    }

    void init(
        GraphicsService* gfx,
        D2D1_RECT_F& rect,
        D2D1_COLOR_F bgColor
    ) {
        this->gfx = gfx;
        setRect(rect);
        setFgColor(black);
        setBgColor(bgColor);
        bitmap = gfx->makeBitmap(w, h);
        updateBitmap();
    }

    void setRect(D2D1_RECT_F rect) {
        this->rect = rect;
        this->w = rect.right - rect.left;
        this->h = rect.bottom - rect.top;
        this->midpoint = h / 2;
    }

    void setFgColor(D2D1_COLOR_F fgColor) {
        this->fgColor = fgColor;
        this->invertedFgColor = makeInvertedColor(fgColor);
    }

    void setBgColor(D2D1_COLOR_F bgColor) {
        this->bgColor = bgColor;
        this->invertedBgColor = makeInvertedColor(bgColor);
    }

    void setWave(AudioBuffer* _wave) {
        if (wave.size() != _wave->size()) {
            wave.resize(_wave->size());
        }

        for (int i = 0; i < wave.size(); i++) {
            wave[i] = (*_wave)[i];
        }

        windowBegin = 0;
        windowEnd = wave.size();
        updateBitmap();
    }

    void zoom(int delta_) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        int delta = delta_ * step;
        windowBegin = clamp(windowBegin + delta, 0, wave.size());
        windowEnd = clamp(windowEnd - delta, 0, wave.size());
        updateBitmap();
    }

    void scroll(int delta_) {
        double windowSize = (double)(windowEnd - windowBegin);
        double step = windowSize / (double)w;
        int delta = delta_ * step;
        windowBegin = clamp(windowBegin + delta, 0, wave.size());
        windowEnd = clamp(windowEnd + delta, 0, wave.size());
        updateBitmap();
    }

    void zoomToSelection() {
        if (selected) {
            int smallerSample = cursor < selectEnd ? cursor : selectEnd;
            int biggerSample = cursor >= selectEnd ? cursor : selectEnd;
            windowBegin = smallerSample;
            windowEnd = biggerSample;
            selected = false;
            updateBitmap();
        }
    }

    void onLeftClick(int x, int y) {
        int cursorPixel = x - rect.left;
        cursor = xPixelToXSample(cursorPixel);
        selected = false;
        updateBitmap();
    }

    void onDrag(int x, int y, int xDelta, int yDelta) {
        int selectEndPixel = x - rect.left;
        selectEnd = xPixelToXSample(selectEndPixel);
        selected = true;
        updateBitmap();
    }

    void draw(int z) {
        gfx->drawBitmap(bitmap, rect, z);
    }

private:
    void updateBitmap() {
        bitmap->fill(bgColor);

        drawHorizontalLine(0, w, midpoint, fgColor);

        if (wave.size() == 0) {
            return;
        }

        double sample = 0.0;
        int yPixel = 0;

        int cursorPixel = xSampleToXPixel(cursor);
        int selectEndPixel = xSampleToXPixel(selectEnd);

        for (int pixelIdx = 0; pixelIdx < w; pixelIdx++) {
            int sampleIdx = xPixelToXSample(pixelIdx);
            sample = inBounds(wave, sampleIdx) ? wave[sampleIdx] : 0.0;

            yPixel = ySampleToYPixel(sample);

            if (selected && inSelection(sampleIdx, cursor, selectEnd)) {
                drawVerticalLine(pixelIdx, 0, h, invertedBgColor);
                drawVerticalLine(pixelIdx, midpoint, yPixel, invertedFgColor);
            } else {
                drawVerticalLine(pixelIdx, midpoint, yPixel, fgColor);
            }
        }

        if (cursorPixel < w && cursorPixel >= 0) {
            drawVerticalLine(cursorPixel, 0, h, fgColor);
        }

        if (selected && selectEndPixel < w && selectEndPixel >= 0) {
            drawVerticalLine(selectEndPixel, 0, h, fgColor);
        }
    }

    int xPixelToXSample(int pixel) {
        double windowSize = (double)(windowEnd - windowBegin);
        double scale = windowSize / (double)w;
        return (pixel * scale) + windowBegin;
    }

    int xSampleToXPixel(int sample) {
        double windowSize = (double)(windowEnd - windowBegin);
        double scale = (double)w / windowSize;
        return (sample - windowBegin) * scale;
    }

    int ySampleToYPixel(double sample) {
        return h - (int)(((sample * 0.5) + 0.5) * h);
    }

    void drawHorizontalLine(int x1, int x2, int y, D2D1_COLOR_F& color) {
        int biggerX = x1 >= x2 ? x1 : x2;
        int smallerX = x1 < x2 ? x1 : x2;

        for (int x = smallerX; x < biggerX; ++x) {
            bitmap->setPixel(x, y, color);
        }
    }

    void drawVerticalLine(int x, int y1, int y2, D2D1_COLOR_F& color) {
        int biggerY = y1 >= y2 ? y1 : y2;
        int smallerY = y1 < y2 ? y1 : y2;

        for (int y = smallerY; y < biggerY; y++) {
            bitmap->setPixel(x, y, color);
        }
    }

    bool inSelection(int x, int s1, int s2) {
        int bigger = s1 >= s2 ? s1 : s2;
        int smaller = s1 < s2 ? s1 : s2;
        return (x >= smaller && x < bigger);
    }
};
