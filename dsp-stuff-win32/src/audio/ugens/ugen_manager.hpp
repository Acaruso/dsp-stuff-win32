#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// windows.h defines `min` and `max` macros which mess up robin_map
// need to undefine `min` and `max` before including robin_map
#undef min
#undef max
#include "src/lib/robin-map/robin_map.h"
#include "src/lib/robin-map/robin_set.h"

#include "src/audio/audio_util.hpp"
#include "src/audio/ugens/base_ugen.hpp"
#include "src/shared/audio_buffer.hpp"

const int MANAGER = -99;

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

    std::vector<unsigned> outBuffers;

    UgenManager(UgenCtx* _ugenCtx, int _numIns, int _numOuts) {
        typeStr = "UgenManager";
        ugenCtx = _ugenCtx;
        numIns = _numIns;
        numOuts = _numOuts;
        allocateBuffers(typeStr);
    }

    void allocateBuffers(std::string str="") override {
        resizeIns(numIns, str);
        resizeOuts(numOuts, str);

        for (int i = 0; i < numOuts; ++i) {
            unsigned newOffset = ugenCtx->bufferAllocator.allocate(str);
            outBuffers.push_back(newOffset);
        }
    }

    int addUgen(BaseUgen* ugen) {
        int id = nextId;
        ugens[id] = ugen;
        ugenIds.push_back(id);
        ++nextId;
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
        BaseUgen* pSource = getUgen(sourceId);
        BaseUgen* pDest = getUgen(destId);

        pSource->assertOutInactive(sourcePort);
        pDest->assertInInactive(destPort);

        edges[sourceId].insert(destId);

        bool success = topoSort();

        if (success) {
            unsigned destOffset = pDest->in[destPort];
            pSource->out[sourcePort] = destOffset;

            pSource->setOutActive(sourcePort, true);
            pDest->setInActive(destPort, true);
        } else {
            edges[sourceId].erase(destId);
        }
    }

    void connect(std::vector<int> v) {
        if (v.size() % 4 != 0) {
            std::cout << "connect vector size is not divisible by 4!";
            exit(1);
        }

        int sourceId;
        int sourcePort;
        int destId;
        int destPort;

        int i = 0;
        while (i < v.size()) {
            sourceId   = v[i++];
            sourcePort = v[i++];
            destId     = v[i++];
            destPort   = v[i++];

            if (sourceId == MANAGER) {
                connectIn(sourcePort, destId, destPort);
            } else if (destId == MANAGER) {
                connectOut(sourceId, sourcePort, destPort);
            } else {
                connect(sourceId, sourcePort, destId, destPort);
            }
        }
    }

    void connectIn(int inPort, int destId, int destPort) {
        BaseUgen* pDest = getUgen(destId);

        pDest->assertInInactive(destPort);

        UgenInRoute inRoute = { destId, inPort, destPort };

        if (std::find(inRoutes.begin(), inRoutes.end(), inRoute) == inRoutes.end()) {
            inRoutes.push_back(inRoute);
            pDest->setInActive(destPort, true);
        }
    }

    void connectOut(int sourceId, int sourcePort, int outPort) {
        BaseUgen* pSource = getUgen(sourceId);

        pSource->assertOutInactive(sourcePort);

        if (outPort >= outBuffers.size()) {
            std::cout << typeStr << ".out[" << outPort << "] doesn't exist!";
            exit(1);
        }

        unsigned outOffset = outBuffers[outPort];
        pSource->out[sourcePort] = outOffset;
        pSource->setOutActive(sourcePort, true);
    }

    void run(unsigned sampleCounter) override {
        auto& data = ugenCtx->bufferAllocator.data;
        BaseUgen* pUgen = nullptr;

        // handle input routing
        for (auto& inRoute : inRoutes) {
            pUgen = getUgen(inRoute.destId);
            copyBuffer(
                data, 
                this->in[inRoute.inPort], 
                bufferSize, 
                pUgen->in[inRoute.destPort]
            );
        }

        // run children ugens
        for (auto id : topoSortedUgens) {
            pUgen = getUgen(id);
            pUgen->run(sampleCounter);
        }

        // write out buffers
        for (int i = 0; i < out.size(); ++i) {
            copyBuffer(data, outBuffers[i], bufferSize, out[i]);
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

        for (const auto id : ugenIds) {
            visited[id] = NOT_VISITED;
        }

        for (const auto id : ugenIds) {
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

        for (auto edge : eltEdges) {
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
