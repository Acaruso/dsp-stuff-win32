#pragma once

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts_intrusive/ui_tree_node.hpp"

class RectEltIntrusive {
public:
    UiTreeNode uiTreeNode;
    bool outline = false;

    RectEltIntrusive(GraphicsService* gfx_, D2D1_RECT_F rect_, bool outline_=false) {
        uiTreeNode.gfx = gfx_;
        uiTreeNode.rect = rect_;
        uiTreeNode.absoluteRect = rect_;
        outline = outline_;

        uiTreeNode.onDraw = [](UiTreeNode* n) {
            RectEltIntrusive* r = (RectEltIntrusive*)n;

            if (r->outline) {
                n->gfx->outlineRect(n->rect, black);
            } else {
                n->gfx->drawRect(n->rect, black);
            }
        };
    }
};
