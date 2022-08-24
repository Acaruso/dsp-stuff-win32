#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

class BaseUgen;

class BaseUgen {
public:
    std::unordered_map<int, double> inSigs;
    std::unordered_map<int, double> outSigs;

    void zeroInSigs() {
        for (auto& [key, value] : inSigs) {
            value = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
