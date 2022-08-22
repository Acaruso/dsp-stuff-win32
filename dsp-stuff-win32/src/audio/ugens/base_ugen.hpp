#pragma once

#include <vector>

class BaseUgen {
public:
    std::vector<BaseUgen*> inputs{4, nullptr};
    std::vector<BaseUgen*> outputs{4, nullptr};
    virtual double get(double t);
};
