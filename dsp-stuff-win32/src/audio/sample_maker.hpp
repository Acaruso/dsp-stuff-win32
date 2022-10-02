#pragma once

#include <string>
#include <vector>

#include "src/audio/ugens/composite/composite_ugens.hpp"
#include "src/audio/ugens/bang.hpp"
#include "src/audio/ugens/sink.hpp"
#include "src/audio/ugens/ugen_manager.hpp"
#include "src/shared/audio_buffer.hpp"
#include "src/shared/shared_data.hpp"

// inline void trigger(BaseUgen* ugen) {
//     ugen->writeIn(0, 0, 1.0f);
// }

inline void trigger(BaseUgen* ugen) {
    ((Bang*)ugen)->doBang();
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

        // sharedData->ugenCtx.bufferAllocator.zeroAll();
        
        for (int i = 0; i < toTriggerSize; ++i) {
            trigger(toTrigger[i]);
        }

        root->run(sampleCounter);

        BaseUgen* outSink = root->getUgen("outSink");

        sharedData->rootUgenLock.unlock();

        return ((Sink*)outSink)->buffer;
    }
};
