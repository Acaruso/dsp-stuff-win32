#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/basic/container_elt.hpp"
#include "src/main/waveform_display.hpp"

class WaveformElt : public ContainerElt {
public:
    WaveformDisplay waveformDisplay;

    WaveformElt(GraphicsService* gfx, D2D1_RECT_F rect) {
        this->gfx = gfx;
        this->rect = rect;
        this->absoluteRect = rect;
        waveformDisplay.init(gfx, rect, green);
    }

    void draw() override {
        waveformDisplay.draw();
    }
};
