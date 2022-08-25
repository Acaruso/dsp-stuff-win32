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

class UgenManager : public BaseUgen {
    template<typename K, typename V>
    using map = std::unordered_map<K, V>;

    template<typename K>
    using set = std::unordered_set<K>;

    using SourceId = int;
    using DestId = int;
    using SourcePort = int;
    using DestPort = int;

    using InPort = int;
    using OutPort = int;

public:
    std::unordered_map<int, BaseUgen*> ugens;
    map<SourceId, map<DestId, map<SourcePort, set<DestPort>>>> edges;
    std::vector<int> topoSortedUgens;
    std::unordered_map<int, TopoSortStatus> visited;
    bool loopDetected = false;
    int nextId = 1;

    map<InPort, map<DestId, set<DestPort>>> inRoutes;
    map<SourceId, map<SourcePort, set<OutPort>>> outRoutes;

    UgenManager() {}

    void connectIn(int inPort, int destId, int destPort) {
        inRoutes[inPort][destId].insert(destPort);
    }

    void connectOut(int sourceId, int sourcePort, int outPort) {
        outRoutes[sourceId][sourcePort].insert(outPort);
    }

    void run(double t) {
        // handle input routing
        for (auto& [inPort, destIdToDestPorts] : inRoutes) {
            for (auto& [destId, destPorts] : destIdToDestPorts) {
                BaseUgen* ugen = getUgen(destId);
                for (auto& destPort : destPorts) {
                    ugen->in[destPort] = this->in[inPort];
                }
            }
        }

        for (auto& id : topoSortedUgens) {
            BaseUgen* ugen = getUgen(id);
            ugen->run(t);
            writeOutputs(id);
        }

        // handle output routing
        for (auto& [sourceId, sourcePortToOutPorts] : outRoutes) {
            BaseUgen* ugen = getUgen(sourceId);
            for (auto& [sourcePort, outPorts] : sourcePortToOutPorts) {
                for (auto& outPort : outPorts) {
                    this->out[outPort] = ugen->out[sourcePort];
                }
            }
        }

        // need to zero ins after each sample because we're SUMMING sample inputs
        zeroIns();
    }

    void zeroIns() {
        for (auto& [key, value] : in) {
            value = 0.0;
        }

        for (auto& id : topoSortedUgens) {
            BaseUgen* ugen = getUgen(id);
            ugen->zeroIns();
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
        topoSort();
        return id;
    }

    BaseUgen* getUgen(int id) {
        return ugens[id];
    }

    void connect(int sourceId, int sourcePort, int destId, int destPort) {
        edges[sourceId][destId][sourcePort].insert(destPort);
        topoSort();
    }

    void disconnect(int sourceId, int sourcePort, int destId, int destPort) {
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

    void topo(int sourceId) {
        if (loopDetected == true) {
            return;
        }

        visited[sourceId] = IN_FLIGHT;

        auto& eltEdges = edges[sourceId];

        for (auto& edge : eltEdges) {
            DestId destId = edge.first;

            if (visited[destId] == IN_FLIGHT) {
                loopDetected = true;
                return;
            } else if (visited[destId] == NOT_VISITED) {
                topo(destId);
            }
        }

        visited[sourceId] = VISITED;
        topoSortedUgens.push_back(sourceId);
    }
};
