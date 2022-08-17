#pragma once

#include <unordered_map>
#include <vector>

#include "src/main/ui_elts/base_elt.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/util.hpp"

class UiEltService {
public:
    GraphicsService* gfx;
    std::unordered_map<int, std::vector<int>> connections;
    int nextId = 0;
    BaseElt* root = nullptr;

    UiEltService(GraphicsService* gfx) : gfx(gfx) {
        root = makeContainerElt(gfx, makeRectF(0, 0, 1000, 1000), nextId++);
    }

    void pushElt(BaseElt* parent, BaseElt* newElt) {
        if (parent == nullptr) {
            root = newElt;
        } else {
            
        }
    }
};
