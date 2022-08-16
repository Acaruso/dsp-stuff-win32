#pragma once

#include <memory>
#include <vector>

#include <d2d1.h>

class BaseElt {
public:
    virtual D2D1_RECT_F getRect() = 0;
    virtual void setRect(D2D1_RECT_F rect) = 0;
    virtual BaseElt* getParent() = 0;
    virtual void setParent(BaseElt* parent) = 0;
    virtual std::vector<BaseElt*>& getChildren() = 0;
    virtual void pushChild(BaseElt* elt) = 0;
    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
