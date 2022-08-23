#pragma once

#include <vector>

class BaseUgen;

struct UgenOutput {
    BaseUgen* ugen = nullptr;
    int destPort = 0;
};

class BaseUgen {
public:
    const int numInputs = 4;
    const int numOutputs = 4;
    std::vector<double> inputs = std::vector<double>(numInputs, 0.0);
    std::vector<UgenOutput> outputs = std::vector<UgenOutput>(numOutputs);

    void addOutput(BaseUgen* ugen, int sourcePort, int destPort) {
        outputs[sourcePort].ugen = ugen;
        outputs[sourcePort].destPort = destPort;
    }

    void writeOutput(int sourcePort, double sig) {
        if (outputs[sourcePort].ugen != nullptr) {
            int destPort = outputs[sourcePort].destPort;
            outputs[sourcePort].ugen->inputs[destPort] = sig;
        }
    }

    virtual void get(double t) = 0;

    virtual ~BaseUgen() = default;
};
