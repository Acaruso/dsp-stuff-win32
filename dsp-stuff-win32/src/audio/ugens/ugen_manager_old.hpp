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
    int nextId = 1;

    UgenManager() {}

    void zeroAllInSigs() {
        for (auto& id : topoSortedUgens) {
            BaseUgen* ugen = getUgen(id);
            ugen->zeroInSigs();
        }
    }

    void runAll(double t) {
        for (auto& id : topoSortedUgens) {
            BaseUgen* ugen = getUgen(id);
            ugen->run(t);
            writeOutputs(id);
        }
    }

    void writeOutputs(int sourceId) {
        BaseUgen* sourceUgen = getUgen(sourceId);

        for (auto& [sourcePort, ugenOuts] : sourceUgen->edges) {
            for (auto& ugenOut : ugenOuts) {
                BaseUgen* destUgen = getUgen(ugenOut.destId);
                int destPort = ugenOut.destPort;

                // TODO: sum input signals instead of overwriting
                destUgen->in[destPort] = sourceUgen->out[sourcePort];
            }
        }
    }

    int addUgen(BaseUgen* ugen) {
        int id = nextId;
        ugens[id] = ugen;
        nextId++;
        return id;
    }

    BaseUgen* getUgen(int id) {
        return ugens[id];
    }

    void addConnection(int sourceId, int sourcePort, int destId, int destPort) {
        BaseUgen* source = getUgen(sourceId);
        bool res = addEdge(sourceId, destId);
        if (res == false) {
            return;
        }
        source->addOutput(sourcePort, destId, destPort);
    }

    void deleteConnection(int sourceId, int sourcePort, int destId, int destPort) {
        getUgen(sourceId)->deleteOutput(sourcePort, destId, destPort);
        deleteEdge(sourceId, destId);
    }

private:
    bool addEdge(int sourceId, int destId) {
        edges[sourceId].insert(destId);
        bool res = topoSort();
        if (res == false) {
            deleteEdge(sourceId, destId);
        }
        return res;
    }

    void deleteEdge(int sourceId, int destId) {
        edges[sourceId].erase(destId);
        topoSort();
    }

    bool topoSort() {
        topoSortedUgens.clear();
        visited.clear();
        loopDetected = false;

        for (const auto& [id, _] : ugens) {
            visited[id] = NOT_VISITED;
        }

        for (const auto& [id, _] : ugens) {
            if (visited[id] == NOT_VISITED) {
                topo(id);
            }
            if (loopDetected == true) {
                return false;
            }
        }

        std::reverse(topoSortedUgens.begin(), topoSortedUgens.end());

        return true;
    }

    void topo(int id) {
        if (loopDetected == true) {
            return;
        }

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
