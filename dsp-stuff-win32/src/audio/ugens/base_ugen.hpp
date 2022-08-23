#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

class BaseUgen;

struct UgenOut {
    int destId = 0;
    int destPort = 0;

};

inline bool operator==(const UgenOut& lhs, const UgenOut& rhs) {
    return (
        (lhs.destId == rhs.destId)
        && (lhs.destPort == rhs.destPort)
    );
}

class BaseUgen {
public:
    std::unordered_map<int, double> inSigs;
    std::unordered_map<int, double> outSigs;
    std::unordered_map<int, std::vector<UgenOut>> edges;

    void addOutput(int sourcePort, int destId, int destPort) {
        UgenOut newUgenOut = { destId, destPort };
        edges[sourcePort].push_back(newUgenOut);
    }

    void deleteOutput(int sourcePort, int destId, int destPort) {
        auto it = edges.find(sourcePort);
        if (it == edges.end()) {
            return;
        }

        std::vector<UgenOut>& ugenOuts = it->second;

        UgenOut toDelete = { destId, destPort };

        auto it2 = std::find(ugenOuts.begin(), ugenOuts.end(), toDelete);
        if (it2 != ugenOuts.end()) {
            ugenOuts.erase(it2);
        }
    }

    void zeroInSigs() {
        for (auto& [key, value] : inSigs) {
            value = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
