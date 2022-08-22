#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"

class UgenManager {
public:
    std::unordered_map<int, BaseUgen*> ugens;
    std::unordered_map<int, std::unordered_set<int>> edges;
    int nextId = 0;

    UgenManager() {}

    int addUgen(BaseUgen* ugen) {
        int id = nextId;
        ugens[id] = ugen;
        nextId++;
        return id;
    }

    BaseUgen* getUgen(int id) {
        return ugens[id];
    }

    void addEdge(int sourceId, int destId) {
        if (edges.find(sourceId) == edges.end()) {
            edges[sourceId] = std::unordered_set<int>();
        }
        edges[sourceId].insert(destId);
    }
};
