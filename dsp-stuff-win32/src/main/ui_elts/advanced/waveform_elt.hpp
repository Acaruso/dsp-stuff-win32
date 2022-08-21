#pragma once

#include <vector>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/input_state.hpp"
#include "src/main/ui_elts/basic/base_elt.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/util.hpp"
#include "src/main/waveform_display.hpp"
#include "src/shared/shared_data.hpp"

class WaveformElt : public BaseElt {
public:
    WaveformDisplay waveformDisplay;
    std::vector<double>* buffer = nullptr;
    SharedData* sharedData = nullptr;

    WaveformElt(
        GraphicsService* gfx_,
        std::vector<double>* buffer_,
        InputState* inputState_,
        SharedData* sharedData_,
        D2D1_RECT_F rect_,
        int z_=0
    ) {
        gfx = gfx_;
        buffer = buffer_;
        inputState = inputState_;
        sharedData = sharedData_;
        rect = rect_;
        absoluteRect = rect_;
        z = z_;

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
        gfx->outlineRect(rect, black, z + 1);
        waveformDisplay.draw(z);
    }

    void onTick() override {
        if (sharedData->envOn) {
            waveformDisplay.setWave(buffer);
        }

        if (inputState->isActiveWindow && isInsideRect(inputState->mouseX, inputState->mouseY, absoluteRect)) {
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
    }
};

