#pragma once

class ContainerElt {
public:
    GraphicsService* gfx = nullptr;
    D2D1_RECT_F rect;

    void draw() {}
};
