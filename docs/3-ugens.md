# overview

- the ugen system is the core DSP engine of the project
- the basic idea: small, composable audio processing units (ugens) are wired together into a graph
- each ugen reads from input buffers, does some processing, and writes to output buffers
- ugens are connected by making a source's output point to the same buffer as a destination's input
- the graph is executed in topologically sorted order so that a ugen's inputs are always ready before it runs

# ugen_ctx.hpp

- `UgenCtx` is not a ugen, its the shared "context" that every ugen has access to via its `ugenCtx` pointer

- `UgenCtx` has three member variables:
  - `BufferAllocator`
    - `BufferAllocator` is used to allocate the input and output buffers used by ugens
      - when a ugen wants to send data to another ugen, it writes it to one of these buffers (explained more later)
    - `BufferAllocator` contains a large vector, `std::vector<float> data`
      - all of the buffers used by the ugen system are "allocated" from `data`
        - to allocate a buffer, a ugen calls `BufferAllocator::allocate`
        - why do it this way? storing all buffer data in one contiguous block improves cache efficiency
    - memory in `data` is never freed
  - `Wavetables`
    - this contains pre-computed lookup tables: sin, tanh, saw, square, triangle, noise, bitcrush
    - each wavetable is 1024 samples
    - oscillators can index into these instead of computing `sin()` etc. at runtime
  - `Waves`
    - loaded audio samples (snare, hi-hat)
    - loaded from wav files at startup by `WaveReader`

# base_ugen.hpp

- this is the base class that all other ugens inherit from

- `BaseUgen` has a few member variables related to audio data input and output:
  - input related:
    - `std::vector<unsigned> in`
      - `in` is a vector of offsets into `BufferAllocator::data`
      - `in` represents the buffers that the ugen uses for its inputs
      - the size of each input buffer is `bufferSize` -- the size of the system's audio buffer
      - example: input buffer `0` is located in `data` in range `[in[0], in[0] + bufferSize)`
    - `std::vector<bool> inActive`
      - `inActive[i] == true` if another ugen's output is connected to this ugen's input `i`
  - output related:
    - `std::vector<unsigned> out`
      - `out` is a vector of offsets into `BufferAllocator::data`
      - these actually correspond to some other ugen's input buffers
        - thus, a ugen only "owns" its input buffers, it doesn't own its output buffers
        - this avoids copying - a ugen writes its output directly to another ugen's input buffer
    - `std::vector<bool> outActive`
      - similar to `inActive`

- how `BaseUgen` is initialized:
  - `BaseUgen` assumes that any classes that inherit from it do certain things in their constructor:
    - they set `numIns` and `numOuts` in their constructor to be their desired number of inputs and outputs
      - `numIns` and `numOuts` are member variables of `BaseUgen`
    - they call `allocateBuffers` at the end of their constructor
    - this assumption seems fragile and like its not a great design
  - `allocateBuffers`
    - calls `resizeIns` and `resizeOuts`
  - `resizeIns`
    - allocates `numIns` buffers from `bufferAllocator` and stores the offsets in `in`
    - resizes `inActive` to be `numIns` size
  - `resizeOuts`
    - resizes `out`
      - we don't need to allocate any buffers
    - resizes `outActive`

# const_value.hpp

- this is a good example of a simple ugen

- `ConstValue::ConstValue`
  - does the expected setup tasks:
    - sets `numOuts = 1`
    - calls `allocateBuffers`

- `ConstValue::run`
  - writes `bufferSize` samples of `value * level` to its output buffer

# ugen_manager.hpp

- `UgenManager` is basically a container for ugens
- all ugens must exist within some `UgenManager`
  - ultimately, the entire ugen graph is contained within `UgenManager rootUgen` which is contained within `SharedData`
  - there could also be more `UgenManagers` nested with `rootUgen` if necessary
  - all ugens must exist with a `UgenManager` because the `UgenManager` handles various important things:
    - it stores the data that describes the connections between ugens, i.e. it stores the graph structure
    - it handles topologically sorting the ugen graph
    - it handles executing the ugen graph


































