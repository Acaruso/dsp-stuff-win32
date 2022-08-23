#pragma once

#include <unordered_map>
#include <vector>

class BaseUgen;

struct UgenOut {
    int destId = 0;
    int destPort = 0;
};

class BaseUgen {
public:
    std::unordered_map<int, double> inSigs;
    std::unordered_map<int, double> outSigs;
    std::unordered_map<int, std::vector<UgenOut>> edges;

    void addOutput(int destId, int sourcePort, int destPort) {
        UgenOut newUgenOut = { destId, destPort };
        edges[sourcePort].push_back(newUgenOut);
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
