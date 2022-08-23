#pragma once

#include <unordered_map>
#include <vector>

class BaseUgen;

struct UgenOut {
    BaseUgen* destUgen = nullptr;
    int destPort = 0;
};

class BaseUgen {
public:
    std::unordered_map<int, double> inSigs;
    std::unordered_map<int, double> outSigs;
    std::unordered_map<int, std::vector<UgenOut>> edges;

    void addOutput(BaseUgen* destUgen, int sourcePort, int destPort) {
        UgenOut newUgenOut = { destUgen, destPort };
        edges[sourcePort].push_back(newUgenOut);
    }

    void writeOutputs() {
        for (auto& [sourcePort, ugenOuts] : edges) {
            for (auto& ugenOut : ugenOuts) {
                BaseUgen* destUgen = ugenOut.destUgen;
                int destPort = ugenOut.destPort;

                // TODO: sum input signals instead of overwriting
                destUgen->inSigs[destPort] = outSigs[sourcePort];
            }
        }
    }

    // void writeOutput(int sourcePort, double sig) {
    //     if (outputs[sourcePort].ugen != nullptr) {
    //         int destPort = outputs[sourcePort].destPort;
    //         outputs[sourcePort].ugen->inputs[destPort] = sig;
    //     }
    // }

    virtual void get(double t) = 0;

    virtual ~BaseUgen() = default;
};
