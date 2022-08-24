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
    template<typename K, typename V>
    using Map = std::unordered_map<K, V>;

    template<typename K>
    using Set = std::unordered_set<K>;

    using SourceId = int;
    using DestId = int;
    using SourcePort = int;
    using DestPort = int;

public:
    std::unordered_map<int, BaseUgen*> ugens;
    Map<SourceId, Map<DestId, Map<SourcePort, Set<DestPort>>>> edges;
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

        auto& _edges = edges[sourceId];

        for (auto& [destId, sourcePortToDestPorts] : _edges) {
            BaseUgen* destUgen = getUgen(destId);

            for (auto& [sourcePort, destPorts] : sourcePortToDestPorts) {

                for (auto& destPort : destPorts) {
                    destUgen->in[destPort] += sourceUgen->out[sourcePort];
                }
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
        edges[sourceId][destId][sourcePort].insert(destPort);
        topoSort();
    }

    void deleteConnection(int sourceId, int sourcePort, int destId, int destPort) {
        bool deleted = false;

        auto sourceIdToDestIds = edges.find(sourceId);

        if (sourceIdToDestIds != edges.end()) {
            auto destIdToSourcePorts = sourceIdToDestIds->second.find(destId);

            if (destIdToSourcePorts != sourceIdToDestIds->second.end()) {
                auto sourcePortToDestPorts = destIdToSourcePorts->second.find(sourcePort);

                if (sourcePortToDestPorts != destIdToSourcePorts->second.end()) {
                    auto iDestPort = sourcePortToDestPorts->second.find(destPort);

                    if (iDestPort != sourcePortToDestPorts->second.end()) {
                        sourcePortToDestPorts->second.erase(iDestPort);
                        deleted = true;

                        if (sourcePortToDestPorts->second.size() == 0) {
                            destIdToSourcePorts->second.erase(sourcePortToDestPorts);

                            if (destIdToSourcePorts->second.size() == 0) {
                                sourceIdToDestIds->second.erase(destIdToSourcePorts);

                                if (sourceIdToDestIds->second.size() == 0) {
                                    edges.erase(sourceIdToDestIds);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (deleted) {
            topoSort();
        }
    }

private:
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
            DestId destId = edge.first;

            if (visited[destId] == IN_FLIGHT) {
                loopDetected = true;
                return;
            } else if (visited[destId] == NOT_VISITED) {
                topo(destId);
            }
        }

        visited[id] = VISITED;
        topoSortedUgens.push_back(id);
    }
};
