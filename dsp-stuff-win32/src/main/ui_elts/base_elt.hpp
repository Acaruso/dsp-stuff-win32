#pragma once

#include <cassert>
#include <vector>

#include <d2d1.h>

#include "src/main/ui_elts/ui_elt_types.hpp"
#include "src/main/ui_elts/container_elt.hpp"
#include "src/main/ui_elts/rect_elt.hpp"
#include "src/main/util.hpp"
#include "src/main/graphics_service.hpp"

// class BaseElt {
// public:
//     UI_ELT_TYPE tag;

//     GraphicsService* gfx = nullptr;

//     union {
//         ContainerElt containerElt;
//         RectElt rectElt;
//     } inner;

//     std::vector<BaseElt> children;

//     void draw() {
//         v_draw();
        
//         for (auto& child : children) {
//             child.draw();
//         }
//     }

//     void v_draw() {
//         switch (tag) {
//             case UI_ELT_CONTAINER: inner.containerElt.draw();
//             case UI_ELT_RECT: inner.rectElt.draw();
//             default: assert(false);
//         }
//     }
// };

// inline BaseElt makeContainerElt(GraphicsService* gfx, D2D1_RECT_F rect) {
//     BaseElt elt = BaseElt{};
//     elt.tag = UI_ELT_CONTAINER;
//     elt.gfx = gfx;
//     elt.inner.containerElt.gfx = gfx;
//     elt.inner.containerElt.rect = rect;
//     return elt;
// }

// inline BaseElt makeRectElt(GraphicsService* gfx, D2D1_RECT_F rect) {
//     BaseElt elt = BaseElt{};
//     elt.tag = UI_ELT_RECT;
//     elt.gfx = gfx;
//     elt.inner.rectElt.gfx = gfx;
//     elt.inner.rectElt.rect = rect;
//     return elt;
// }

class BaseElt {
public:
    UI_ELT_TYPE tag;

    GraphicsService* gfx = nullptr;

    int id = 0;

    union U {
        ContainerElt containerElt;
        RectElt rectElt;
        U() : containerElt() {}      // default union constructor -- by default, union contains containerElt
    } inner;

    std::vector<BaseElt> children;

    void draw() {
        v_draw();
        
        for (auto& child : children) {
            child.draw();
        }
    }

    void v_draw() {
        switch (tag) {
            case UI_ELT_CONTAINER: inner.containerElt.draw();
            case UI_ELT_RECT: inner.rectElt.draw();
            default: assert(false);
        }
    }
};

inline BaseElt* makeContainerElt(GraphicsService* gfx, D2D1_RECT_F rect, int id) {
    BaseElt* elt = new BaseElt;
    elt->tag = UI_ELT_CONTAINER;
    elt->gfx = gfx;
    elt->id = id;
    elt->inner.containerElt.gfx = gfx;
    elt->inner.containerElt.rect = rect;
    return elt;
}

inline BaseElt* makeRectElt(GraphicsService* gfx, D2D1_RECT_F rect, int id) {
    BaseElt* elt = new BaseElt;
    elt->tag = UI_ELT_RECT;
    elt->gfx = gfx;
    elt->id = id;
    elt->inner.rectElt.gfx = gfx;
    elt->inner.rectElt.rect = rect;
    return elt;
}
