#pragma once

#include <vector>

#include "src/audio/ugens/ugen_manager.hpp"

inline void connectSplitOut(UgenManager* m, int splitId, std::vector<int> destIds, int destPort) {
    int i = 0;
    for (auto destId : destIds) {
        m->connect(splitId, i++, destId, destPort);
    }
}

inline void connectSumIn(UgenManager* m, std::vector<int> sourceIds, int sourcePort, int sumId) {
    int i = 0;
    for (auto sourceId : sourceIds) {
        m->connect(sourceId, sourcePort, sumId, i++);
    }
}
