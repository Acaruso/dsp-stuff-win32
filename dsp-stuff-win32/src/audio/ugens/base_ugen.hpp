#pragma once

#include <algorithm>
#include <vector>

class BaseUgen;

class BaseUgen {
public:
    std::vector<double> in = std::vector<double>(4, 0.0);
    std::vector<double> out = std::vector<double>(4, 0.0);

    virtual void zeroIns() {
        for (auto& elt : in) {
            elt = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
