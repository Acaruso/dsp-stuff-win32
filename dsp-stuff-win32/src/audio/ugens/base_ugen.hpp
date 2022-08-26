#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#undef min
#undef max
#include "src/lib/robin-map/robin_map.h"

class BaseUgen;

class BaseUgen {
    // template<typename K, typename V>
    // using map = std::unordered_map<K, V>;

    template<typename K, typename V>
    using map = tsl::robin_map<K, V>;

public:
    map<int, double> in;
    map<int, double> out;

    // std::unordered_map:
    // virtual void zeroIns() {
    //     for (auto& [key, value] : in) {
    //         value = 0.0;
    //     }
    // }

    // tsl::robin_map:
    virtual void zeroIns() {
        for (auto it = in.begin(); it != in.end(); ++it) {
            it.value() = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
