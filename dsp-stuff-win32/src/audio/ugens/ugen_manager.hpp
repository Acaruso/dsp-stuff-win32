#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "src/audio/ugens/base_ugen.hpp"

enum TopoSortStatus {
    NOT_VISITED,
    IN_FLIGHT,
    VISITED
};

class UgenManager {
public:
    std::unordered_map<int, BaseUgen*> ugens;
    std::unordered_map<int, std::unordered_set<int>> edges;
    std::vector<int> topoSortedUgens;
    std::unordered_map<int, TopoSortStatus> visited;
    bool loopDetected = false;

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

    void topoSort() {
        topoSortedUgens.clear();
        visited.clear();
        loopDetected = false;

        for (auto& it : ugens) {
            visited[it.first] = NOT_VISITED;
        }

        for (auto& it : ugens) {
            if (visited[it.first] == NOT_VISITED) {
                topo(it.first);
            }
        }

        std::reverse(topoSortedUgens.begin(), topoSortedUgens.end());
    }

    void topo(int id) {
        visited[id] = IN_FLIGHT;

        auto& eltEdges = edges[id];
        for (auto& edge : eltEdges) {
            if (visited[edge] == IN_FLIGHT) {
                loopDetected = true;
                return;
            } else if (visited[edge] == NOT_VISITED) {
                topo(edge);
            }
        }

        visited[id] = VISITED;
        topoSortedUgens.push_back(id);
    }
};
