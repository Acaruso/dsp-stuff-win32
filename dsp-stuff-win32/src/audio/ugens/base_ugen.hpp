#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

class BaseUgen;

class BaseUgen {
public:
    std::unordered_map<int, double> in;
    std::unordered_map<int, double> out;

    virtual void zeroIns() {
        for (auto& [key, value] : in) {
            value = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
