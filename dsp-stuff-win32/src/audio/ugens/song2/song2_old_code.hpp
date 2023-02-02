    // void run(unsigned sampleCounter) override {
    //     auto& d = ugenCtx->bufferAllocator.data;
    //     unsigned out0 = out[0];

    //     for (int i = 0; i < bufferSize; ++i) {
    //         if (seq.get()) {
    //             wt.setFreq(notes.getFreq());
    //             wtMod.setFreq(notes.getFreq() * 1.0);
    //             notes.incNote();
    //             env.trigger();
    //             envMod.trigger();
    //         }

    //         wt.setPhaseMod(wtMod.get() * envMod.get() * 8);
    //         wtSig = wt.get() * level;
    //         outSig = wtSig * env.get() * level;
    //         WRITE_OUT(d, out0, i, outSig);

    //         runAll();
    //     }
    // }

    // void run(unsigned sampleCounter) override {
    //     auto& d = ugenCtx->bufferAllocator.data;
    //     unsigned out0 = out[0];

    //     for (int i = 0; i < bufferSize; ++i) {
    //         if (seq.get()) {
    //             polyWt.setFreqs(
    //                 notes.makeMajorChord(noteCounter)
    //             );
    //             polyWt.trigger();
    //             env.trigger();
    //             noteCounter = (noteCounter + 5) % 12;
    //         }

    //         wtSig = polyWt.get() * level;
    //         outSig = wtSig * env.get() * level;
    //         WRITE_OUT(d, out0, i, outSig);

    //         runAll();
    //     }
    // }
