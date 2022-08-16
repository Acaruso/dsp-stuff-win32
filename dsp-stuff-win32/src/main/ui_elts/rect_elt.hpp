#pragma once

#include <memory>
#include <vector>

#include <d2d1.h>

#include "src/main/constants.hpp"
#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"
#include "src/main/util.hpp"

class RectElt : public BaseElt {
public:
    GraphicsService* gfx;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;
    D2D1_RECT_F rect;

    RectElt(GraphicsService* gfx, D2D1_RECT_F rect) : gfx(gfx), rect(rect) {}

    virtual D2D1_RECT_F getRect() override {
        return rect;
    }

    virtual void setRect(D2D1_RECT_F rect) override {
        this->rect = rect;
    }

    BaseElt* getParent() override {
        return parent;
    }

    void setParent(BaseElt* parent) override {
        this->parent = parent;
    }

    std::vector<BaseElt*>& getChildren() override {
        return children;
    }
    
    virtual void pushChild(BaseElt* child) override {
        child->setParent(this);
        children.push_back(child);
    }

    void draw() override {
        gfx->drawRect(rect, black);

        for (auto& child : children) {
            child->draw();
        }
    }
};
