#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/waveform_display.hpp"
#include "src/shared/shared_data.hpp"

class WaveformElt : public BaseElt {
public:
    WaveformDisplay waveformDisplay;
    SharedData* sharedData = nullptr;

    WaveformElt(GraphicsService* gfx_, D2D1_RECT_F rect_) {
        gfx = gfx_;
        rect = rect_;
        absoluteRect = rect_;

        waveformDisplay.init(gfx, rect, green);

        onLeftClick = [&](int x, int y) {
            waveformDisplay.onLeftClick(x, y);
        };

        onLeftDrag = [&](int x, int y, int xDelta, int yDelta) {
            waveformDisplay.onDrag(x, y, xDelta, yDelta);
        };

        onMouseWheel = [&](int wheelDelta) {
            if (wheelDelta < 0) {
                waveformDisplay.zoom(-40);
            } else {
                waveformDisplay.zoom(40);
            }
        };

        onKeyDown = [&](int keyCode) {
            if (keyCode == VK_SPACE) {
                sharedData->toAudio.enqueue("trig");
            } else if (keyCode == int('Z')) {
                waveformDisplay.zoomToSelection();
            }
        };
    }

    void onDraw() override {
        gfx->outlineRect(rect, black);
        waveformDisplay.draw();
    }

    void onTick() override {
        if (sharedData->envOn) {
            waveformDisplay.setWave(sharedData->sampleBuffer);
        }

        if (getKeyState(VK_UP)) {
            waveformDisplay.zoom(20);
        }

        if (getKeyState(VK_DOWN)) {
            waveformDisplay.zoom(-20);
        }

        if (getKeyState(VK_LEFT)) {
            waveformDisplay.scroll(-10);
        }

        if (getKeyState(VK_RIGHT)) {
            waveformDisplay.scroll(10);
        }
    }
};

