#pragma once

#include <algorithm>
#include <vector>

#include "src/audio/audio_constants.hpp"

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
    int bufferSize = samplesPerSec / 100;

    std::vector<std::vector<double>> in = {
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0)
    };

    std::vector<std::vector<double>> out = {
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0),
        std::vector<double>(bufferSize, 0.0)
    };
    
    std::vector<UgenConnection> connections;

    void connect(UgenConnection connection) {
        if (std::find(connections.begin(), connections.end(), connection) == connections.end()) {
            connections.push_back(connection);
        }
    }

    void zeroIns() {
        for (auto& inVec : in) {
            for (auto& inDouble : inVec) {
                inDouble = 0.0;
            }
        }
    }

    virtual void run(unsigned sampleCounter) = 0;

    virtual ~BaseUgen() = default;
};
