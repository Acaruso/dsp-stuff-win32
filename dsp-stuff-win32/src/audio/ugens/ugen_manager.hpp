#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// windows.h defines `min` and `max` macros which mess up robin_map
// need to undefine `min` and `max` before including robin_map
#undef min
#undef max
#include "src/lib/robin-map/robin_map.h"
#include "src/lib/robin-map/robin_set.h"

#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"

struct UgenInRoute {
    int destId;
    int inPort;
    int destPort;

    bool operator==(const UgenInRoute& other) const {
        return (
            destId == other.destId
            && inPort == other.inPort
            && destPort == other.destPort
        );
    }
};

enum TopoSortStatus {
    NOT_VISITED,
    IN_FLIGHT,
    VISITED
};

inline void sumCopy(AudioBuffer& dest, AudioBuffer& source) {
    for (int i = 0; i < dest.size(); ++i) {
        dest[i] += source[i];
    }
}

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
    map<std::string, int> ugenNames;
    std::vector<UgenInRoute> inRoutes;

    map<SourceId, set<DestId>> edges;
    std::vector<int> topoSortedUgens;
    map<int, TopoSortStatus> visited;
    bool loopDetected = false;
    int nextId = 0;

    std::vector<AudioBuffer> outBuffers = std::vector<AudioBuffer>(4, AudioBuffer(bufferSize, 0.0f));

    UgenManager() {
        resizeIns(4);
        resizeOuts(4);
    }

    int addUgen(BaseUgen* ugen) {
        int id = nextId;
        ugens[id] = ugen;
        ugenIds.push_back(id);
        nextId++;
        ugen->ugenCtx = ugenCtx;
        topoSort();
        return id;
    }

    int addUgen(std::string name, BaseUgen* ugen) {
        int id = addUgen(ugen);
        addName(name, id);
        return id;
    }

    BaseUgen* getUgen(int id) {
        return ugens[id];
    }

    BaseUgen* getUgen(std::string name) {
        int id = ugenNames[name];
        return ugens[id];
    }

    int getUgenId(std::string name) {
        int id = ugenNames[name];
        return id;
    }

    void addName(std::string name, int id) {
        ugenNames[name] = id;
    }

    void connect(int sourceId, int sourcePort, int destId, int destPort) {
        edges[sourceId].insert(destId);

        bool success = topoSort();

        if (success) {
            BaseUgen* pSource = getUgen(sourceId);
            BaseUgen* pDest = getUgen(destId);
            AudioBuffer* pDestBuffer = &pDest->in[destPort];
            pSource->out[sourcePort].push_back(pDestBuffer);
        } else {
            edges[sourceId].erase(destId);
        }
    }

    void connectIn(int inPort, int destId, int destPort) {
        UgenInRoute inRoute = { destId, inPort, destPort };

        if (std::find(inRoutes.begin(), inRoutes.end(), inRoute) == inRoutes.end()) {
            inRoutes.push_back(inRoute);
        }
    }

    void connectOut(int sourceId, int sourcePort, int outPort) {
        AudioBuffer* pOutBuffer = &outBuffers[outPort];
        BaseUgen* pSource = getUgen(sourceId);
        pSource->out[sourcePort].push_back(pOutBuffer);
    }

    void run(unsigned sampleCounter) {
        zeroOutBuffers();

        BaseUgen* ugen = nullptr;

        // handle input routing
        for (auto& inRoute : inRoutes) {
            ugen = getUgen(inRoute.destId);
            sumCopy(ugen->in[inRoute.destPort], this->in[inRoute.inPort]);
        }

        // run children ugens
        for (auto& id : topoSortedUgens) {
            ugen = getUgen(id);
            ugen->run(sampleCounter);
        }

        writeOutBuffers();

        zeroIns();
        for (auto& id : ugenIds) {
            ugen = getUgen(id);
            ugen->zeroIns();
        }
    }

    void zeroOutBuffers() {
        for (auto& buffer : outBuffers) {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
        }
    }

    void writeOutBuffers() {
        for (int i = 0; i < out.size(); i++) {
            auto& pDestBuffers = out[i];

            for (AudioBuffer* pDestBuffer : pDestBuffers) {
                sumCopy(*pDestBuffer, outBuffers[i]);
            }
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
