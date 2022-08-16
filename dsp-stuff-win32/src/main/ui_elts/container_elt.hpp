#pragma once

#include <memory>
#include <vector>

#include <d2d1.h>

#include "src/main/graphics_service.hpp"
#include "src/main/ui_elts/base_elt.hpp"

class ContainerElt : public BaseElt {
public:
    GraphicsService* gfx;
    D2D1_RECT_F rect;
    D2D1_RECT_F accumRect;
    BaseElt* parent = nullptr;
    std::vector<BaseElt*> children;

    ContainerElt(GraphicsService* gfx, D2D1_RECT_F rect) : gfx(gfx), rect(rect) {}
    ~ContainerElt() {}

    std::vector<BaseElt*>& getChildren() override {
        return children;
    }

    BaseElt* getParent() override {
        return parent;
    }

    void setParent(BaseElt* parent) override {
        this->parent = parent;
    }

    virtual void pushChild(BaseElt* child) override {
        child->setParent(this);
        children.push_back(child);
    }

    virtual D2D1_RECT_F getRect() override {
        return rect;
    }

    virtual void setRect(D2D1_RECT_F rect) override {
        this->rect = rect;
    }

    void draw() override {
        for (auto& child : children) {
            child->draw();
        }
    }
};
