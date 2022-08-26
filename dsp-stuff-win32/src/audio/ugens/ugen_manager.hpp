#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// windows.h defines min and max macros which mess up robin_map
// need to undefine them before including robin_map
#undef min
#undef max
#include "src/lib/robin-map/robin_map.h"
#include "src/lib/robin-map/robin_set.h"

#include "src/audio/ugens/base_ugen.hpp"

enum TopoSortStatus {
    NOT_VISITED,
    IN_FLIGHT,
    VISITED
};

class UgenManager : public BaseUgen {
    // template<typename K, typename V>
    // using map = std::unordered_map<K, V>;

    // template<typename K>
    // using set = std::unordered_set<K>;

    template<typename K, typename V>
    using map = tsl::robin_map<K, V>;

    template<typename K>
    using set = tsl::robin_set<K>;

    using SourceId = int;
    using DestId = int;
    using SourcePort = int;
    using DestPort = int;

    using InPort = int;
    using OutPort = int;

public:
    std::vector<BaseUgen*> ugens = std::vector<BaseUgen*>(128, nullptr);
    std::vector<int> ugenIds;

    map<SourceId, set<DestId>> edges;
    
    std::vector<int> topoSortedUgens;
    map<int, TopoSortStatus> visited;
    bool loopDetected = false;
    int nextId = 0;

    map<InPort, map<DestId, set<DestPort>>> inRoutes;
    map<SourceId, map<SourcePort, set<OutPort>>> outRoutes;

    UgenManager() {}

    int addUgen(BaseUgen* ugen) {
        int id = nextId;
        ugens[id] = ugen;
        ugenIds.push_back(id);
        nextId++;
        topoSort();
        return id;
    }

    BaseUgen* getUgen(int id) {
        return ugens[id];
    }

    void connect(int sourceId, int sourcePort, int destId, int destPort) {
        edges[sourceId].insert(destId);
        getUgen(sourceId)->connect(UgenConnection{destId, sourcePort, destPort});
        topoSort();
    }

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

        for (auto& id : ugenIds) {
            BaseUgen* ugen = getUgen(id);
            ugen->zeroIns();
        }
    }

    void writeOutputs(int sourceId) {
        BaseUgen* sourceUgen = getUgen(sourceId);

        auto& _connections = sourceUgen->connections;

        for (auto& conn : _connections) {
            BaseUgen* destUgen = getUgen(conn.destId);
            destUgen->in[conn.destPort] += sourceUgen->out[conn.sourcePort];
        }
    }

    // TODO: rewrite to work with robin_map

    // void disconnect(int sourceId, int sourcePort, int destId, int destPort) {
    //     bool deleted = false;

    //     auto sourceIdToDestIds = connections.find(sourceId);

    //     if (sourceIdToDestIds != connections.end()) {
    //         auto destIdToSourcePorts = sourceIdToDestIds->second.find(destId);

    //         if (destIdToSourcePorts != sourceIdToDestIds->second.end()) {
    //             auto sourcePortToDestPorts = destIdToSourcePorts->second.find(sourcePort);

    //             if (sourcePortToDestPorts != destIdToSourcePorts->second.end()) {
    //                 auto iDestPort = sourcePortToDestPorts->second.find(destPort);

    //                 if (iDestPort != sourcePortToDestPorts->second.end()) {
    //                     sourcePortToDestPorts->second.erase(iDestPort);
    //                     deleted = true;

    //                     if (sourcePortToDestPorts->second.size() == 0) {
    //                         destIdToSourcePorts->second.erase(sourcePortToDestPorts);

    //                         if (destIdToSourcePorts->second.size() == 0) {
    //                             sourceIdToDestIds->second.erase(destIdToSourcePorts);

    //                             if (sourceIdToDestIds->second.size() == 0) {
    //                                 connections.erase(sourceIdToDestIds);
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     if (deleted) {
    //         topoSort();
    //     }
    // }

private:
    bool topoSort() {
        topoSortedUgens.clear();
        visited.clear();
        loopDetected = false;

        for (const auto& id : ugenIds) {
            visited[id] = NOT_VISITED;
        }

        for (const auto& id : ugenIds) {
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
            DestId destId = edge;

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
