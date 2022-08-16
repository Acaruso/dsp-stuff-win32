#pragma once

#include <memory>
#include <vector>

class BaseElt {
public:
    // std::vector<std::unique_ptr<BaseElt>> children;
    // int x = 0;
    // int y = 0;
    // int w = 0;
    // int h = 0;

    virtual std::vector<std::unique_ptr<BaseElt>>& getChildren() = 0;
    virtual void draw() = 0;
    virtual ~BaseElt() = default;
};
