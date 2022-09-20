#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/sink.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/shared/audio_buffer.hpp"
#include "src/shared/shared_data.hpp"

inline void trigger(BaseUgen* ugen) {
    ugen->in[0][0] = 1.0f;
}

class SampleMaker {
public:
    SharedData* sharedData = nullptr;
    UgenManager* root = nullptr;

    std::vector<BaseUgen*> toTrigger = std::vector<BaseUgen*>(64, nullptr);
    int toTriggerSize = 0;

    void init(SharedData* _sharedData) {
        sharedData = _sharedData;
        root = &sharedData->rootUgen;
    }

    AudioBuffer& makeSamples(unsigned long sampleCounter) {
        sharedData->rootUgenLock.lock();
        
        for (int i = 0; i < toTriggerSize; ++i) {
            trigger(toTrigger[i]);
        }

        root->run(sampleCounter);

        BaseUgen* outSink = root->getUgen("outSink");

        sharedData->rootUgenLock.unlock();

        return ((Sink*)outSink)->buffer;
    }
};
