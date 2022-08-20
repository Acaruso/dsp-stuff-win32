#pragma once

#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts_intrusive/ui_tree_node.hpp"

void handleDraw(GraphicsService* gfx, UiTreeNode* node) {
    node->onDraw(node);

    for (auto child : node->children) {
        handleDraw(gfx, child);
    }
}
