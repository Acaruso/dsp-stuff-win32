#pragma once

#include <algorithm>
#include <vector>

class BaseUgen;

struct UgenConnection {
    int destId;
    int sourcePort;
    int destPort;
    bool operator==(const UgenConnection& other) const {
        return (
            destId == other.destId
            && sourcePort == other.sourcePort
            && destPort == other.destPort
        );
    }
};

class BaseUgen {
public:
    std::vector<double> in = std::vector<double>(4, 0.0);
    std::vector<double> out = std::vector<double>(4, 0.0);
    std::vector<UgenConnection> connections;

    void connect(UgenConnection connection) {
        if (std::find(connections.begin(), connections.end(), connection) == connections.end()) {
            connections.push_back(connection);
        }
    }

    void zeroIns() {
        for (auto& elt : in) {
            elt = 0.0;
        }
    }

    virtual void run(double t) = 0;

    virtual ~BaseUgen() = default;
};
