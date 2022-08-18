#pragma once

#include "src/main/ui_elts_new/base_elt_new.hpp"

struct RectEltData {
    bool outline = false;
};

BaseEltNew* makeRectUiElt(GraphicsService* gfx_, D2D1_RECT_F rect_, bool outline_=false) {
    BaseEltNew* elt = new BaseEltNew(gfx_, rect_);
    RectEltData* rectEltData = new RectEltData;
    rectEltData->outline = outline_;
    elt->data = rectEltData;

    elt->onDraw = [](BaseEltNew* pThis) {
        RectEltData* data = (RectEltData*)(pThis->data);

        if (data->outline) {
            pThis->gfx->outlineRect(pThis->rect, black);
        } else {
            pThis->gfx->drawRect(pThis->rect, black);
        }
    };

    return elt;
}
