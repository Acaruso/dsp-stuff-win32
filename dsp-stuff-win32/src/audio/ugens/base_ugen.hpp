#pragma once

#include <vector>

class BaseUgen;

struct UgenOutput {
    BaseUgen* ugen = nullptr;
    int inputIdx = 0;
};

class BaseUgen {
public:
    const int numInputs = 4;
    const int numOutputs = 4;
    std::vector<double> inputs = std::vector<double>(numInputs, 0.0);
    std::vector<UgenOutput> outputs = std::vector<UgenOutput>(numOutputs);

    void addOutput(BaseUgen* ugen, int outputIdx, int inputIdx) {
        outputs[outputIdx].ugen = ugen;
        outputs[outputIdx].inputIdx = inputIdx;
    }

    void writeOutput(int outputIdx, double sig) {
        if (outputs[outputIdx].ugen != nullptr) {
            int inputIdx = outputs[outputIdx].inputIdx;
            outputs[outputIdx].ugen->inputs[inputIdx] = sig;
        }
    }

    virtual void get(double t) = 0;

    virtual ~BaseUgen() = default;
};
