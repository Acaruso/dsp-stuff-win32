- remove unused headers
- in the UI/graphics code, cache brushes instead of repeatedly re-creating them
- move `sampleCounter` to `ugenCtx` so that we don't have to pass it into `run`
  - also simplify interactions between `AudioService` and `SampleMaker`. maybe remove `SampleMaker` completely and move its functionality into `AudioService`.
- fix issues with how ugens are constructed - there are a lot of very fragile assumptions
  - we assume that the derived class's constructor sets `numIns` and `numOuts` and calls `allocateBuffers`
- update `READ_IN` and `WRITE_OUT` macros
  - both `READ_IN` and `WRITE_OUT` can be updated to take fewer inputs
  - `WRITE_OUT` can also be updated to check `outActive` before writing. this fixes a bug:
    - currently if a ugen is created and its output is not wired up to anything, then it will write its output to the default value of `0`, i.e. it will write its output to `bufferAllocator.data[0]`
    - instead, we should check `outActive` and if the output is not active, i.e. it has not been wired up to anything, we do not write any output
  - ex:
    ```cpp
    #define WRITE_OUT(idx, val) \
        if (outActive[idx]) { ugenCtx->bufferAllocator.data[out[idx] + bufIdx] = val; }
    ```
- refactor ugens to avoid needing to cast from base class to derived class
  - example of problem: `dsp-stuff-win32\src\main\ui_elts\screens\lambda_seq_grid_screen.hpp:makeSeq`
    - we currently do something like this:
      ```cpp
      AHRExpEnvScale* p_freq = (AHRExpEnvScale*)(p_kick->getUgen("freqEnv"))
      p_freq->setAttack(100)
      ```
  - potential solution:
    - each ugen stores a set of "params" as key-value pairs of `string, float`
    - add `params` to `BaseUgen`:
      - `unordered_map<string, float> params`
      - `void setParam(string, float)`
      - we could also use an enum instead of a string as the key
    - each derived class has some set of params that it expects to exist in `params`
    - it uses these params however it needs to, in order to do its DSP
      - in the example of an AHR envelope, these would be "attack", "decay", "release", etc.
    - calling code can set these params by calling `setParam` on the base class
      - thus, we don't need to cast to the derived type
      - however, we still need to "know" the actual derived type of the object in order to know what params are available
        - potential solution:
          - give each derived class an enum member variable which encodes its "actual" type
