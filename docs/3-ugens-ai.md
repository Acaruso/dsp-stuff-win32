- this is the AI generated ugens doc

# overview

- the ugen system is the core DSP engine of the project
- "ugen" stands for "unit generator"
- the basic idea: small, composable audio processing units are wired together into a graph
  - each ugen reads from input buffers, does some processing, and writes to output buffers
  - ugens are connected by making a source's output point to the same buffer as a destination's input
  - the graph is executed in topologically sorted order so that a ugen's inputs are always ready before it runs

# ugen_ctx.hpp

- `UgenCtx` is the shared context that every ugen has access to via its `ugenCtx` pointer

- `UgenCtx` has three member variables:
  - `BufferAllocator`
    - `BufferAllocator` is used to allocate the input and output buffers used by ugens
      - when a ugen wants to send data to another ugen, it writes it to one of these buffers (explained more later)
    - `BufferAllocator` contains `std::vector<float> data`
      - this is large vector
      - all of the buffers used by the ugen system are "allocated" from `data`
        - this is done to improve cache efficiency
        - to allocate a buffer, a ugen calls `BufferAllocator::allocate`
    - memory is never freed
  - `Wavetables`
    - this contains pre-computed lookup tables: sin, tanh, saw, square, triangle, noise, bitcrush
    - each wavetable is 1024 samples
    - oscillators can index into these instead of computing `sin()` etc. at runtime
  - `Waves`
    - loaded audio samples (snare, hi-hat)
    - loaded from wav files at startup by `WaveReader`

# base_ugen.hpp

- `BaseUgen` is the abstract base class for every ugen

- key member variables:
  - `in`
    - vector of `unsigned` values
    - these are **offsets** into `BufferAllocator::data`. so `in[0] = 960` means "my first input reads from index 960 in the shared float array"
  - `out`
    - same as `in`, but for outputs
  - `inActive` / `outActive`
    - track whether each port is connected
  - `ugenCtx`
    - pointer to the shared `UgenCtx`
  - `level`
    - output level multiplier
  - `numIns` / `numOuts`
    - number of input and output ports

- `allocateBuffers`
  - called in each ugen's constructor
  - allocates buffer space from `BufferAllocator` for each input port
  - output ports get offsets but don't allocate their own buffers (they get pointed to a destination's input buffer when connected)

- `run(unsigned sampleCounter)`
  - this is a pure virtual method
  - each subclass implements this to implement their DSP
  - this processes one buffer's worth of audio (480 samples, ~10ms)
  - `sampleCounter` is the global sample position, useful for sequencers but ignored by most ugens

- `READ_IN` and `WRITE_OUT` macros
  - `READ_IN(data, offset, sampleIdx)` reads `data[offset + sampleIdx]`
  - `WRITE_OUT(data, offset, sampleIdx, sample)` writes `data[offset + sampleIdx] = sample`
  - these index into `ugenCtx->bufferAllocator.data` at the port's offset

# how connections work

- when `UgenManager::connect(sourceId, sourcePort, destId, destPort)` is called:
  ```cpp
  unsigned destOffset = pDest->in[destPort];
  pSource->out[sourcePort] = destOffset;
  ```
- the source's output offset is set to the **same value** as the destination's input offset
- so when the source writes to `out[sourcePort]`, it's writing directly into the destination's `in[destPort]` buffer
- thus, there is **no copying** ugen outs and ins share the same region in the flat array
- this is the key efficiency trick: connecting two ugens is just making them point to the same buffer slice

# ugen_manager.hpp

- `UgenManager` is both a **container** for other ugens and a `BaseUgen` itself (it inherits from `BaseUgen`)
  - this means that a `UgenManager` can be added as a child of another `UgenManager`
  - the root `UgenManager` lives in `SharedData::rootUgen`

- key member variables:
  - `ugens` -- vector of 128 `BaseUgen*` slots (pre-allocated, filled with nullptr)
  - `ugenIds` -- list of which slots are occupied
  - `ugenNames` -- map from string names to ids (e.g. `"outSum"` -> `0`)
  - `edges` -- map from source id to set of destination ids (the graph structure)
  - `topoSortedUgens` -- the execution order, computed by topological sort

- `addUgen(ugen)` / `addUgen(name, ugen)`
  - adds a ugen to the next available slot
  - re-runs the topological sort

- `connect(sourceId, sourcePort, destId, destPort)`
  - asserts both ports are inactive (not already connected)
  - adds an edge to the graph
  - re-runs the topological sort
  - if sort succeeds (no cycle), makes the source's output point to the destination's input buffer
  - if sort detects a cycle, removes the edge

- `connect(vector<int>)` -- batch connect
  - takes a flat vector of `[sourceId, sourcePort, destId, destPort, ...]`
  - the special value `MANAGER` (-99) means "this UgenManager's own input/output port"
    - `connectIn` -- routes the manager's input to a child ugen's input (copies data)
    - `connectOut` -- routes a child ugen's output to the manager's output

- `UgenManager::run(sampleCounter)`
  1. copies data from the manager's inputs to child ugens' inputs (via `inRoutes`)
  2. runs each child ugen in topologically sorted order
  3. copies data from child ugens' outputs to the manager's outputs

- topological sort
  - uses DFS with three states: `NOT_VISITED`, `IN_FLIGHT`, `VISITED`
  - `IN_FLIGHT` detection catches cycles -- if we visit a node that's already in-flight, there's a loop
  - the result is reversed at the end (standard topo sort)
  - re-runs every time a ugen is added or a connection is made

# individual ugens

- each ugen follows the same pattern:
  1. constructor sets `numIns`, `numOuts`, calls `allocateBuffers`
  2. `run()` reads from inputs, does processing, writes to outputs

- **ConstValue** -- simplest ugen. outputs a constant value every sample.

- **Bang** -- triggered from the UI thread via `AM_TRIG` message. outputs `1.0f` on the first sample of the buffer when triggered, `0.0f` otherwise. used to fire envelopes, reset oscillator phases, etc.

- **Sum** -- adds all its inputs together. the `outSum` ugen in the root graph is a `Sum` -- everything connects into it.

- **Mult** -- multiplies two inputs together. used as a VCA (voltage-controlled amplifier) -- multiply an oscillator's output by an envelope to shape its volume.

- **Split** -- copies one input to multiple outputs. needed because each output port can only connect to one destination.

- **Sink** -- the output extraction point. copies its input into its own private `AudioBuffer` member variable (not in `BufferAllocator`). this is where `SampleMaker` reads the final audio from. the private buffer is necessary because `BufferAllocator` data gets zeroed/overwritten each cycle.

- **WavetableOsc** -- oscillator that reads from a wavetable
  - has a `phase` accumulator that advances each sample by `fSizexSecondsPerSample * freq`
  - phase wraps around the wavetable size
  - uses `LERP_WT` for linear interpolation between wavetable samples
  - `in[0]` resets phase (used for triggering), `in[1]` adds to phase (for phase modulation)

- **AHRExpEnv** -- attack-hold-release envelope with exponential curves
  - triggered by receiving `1.0f` on `in[0]`
  - attack uses `sqrt` curve (fast rise, slow approach to 1.0)
  - release uses `x^2` curve (fast drop, slow approach to 0.0)
  - `out[0]` is the envelope signal, `out[1]` is on/off (1.0 while active, 0.0 when done)
  - attack/hold/release times are specified in milliseconds, converted to samples internally

- **TriggerSeq** -- step sequencer
  - has a grid of `patterns` (tracks x 16 steps)
  - each output corresponds to one track
  - uses `n16counter` to count samples within a 16th note, `patternCounter` to track the current step
  - outputs `1.0f` trigger pulses at the start of each active step

- **Scale** -- scales a signal from one range to another

- **Recorder** -- records its input into a `SharedAudioBuffer` for waveform display in the UI

# composite ugens (composite_ugens.hpp)

- these are **factory functions** that build common sub-graphs using `UgenManager`
- since `UgenManager` is itself a `BaseUgen`, the result can be used as a single ugen in a larger graph
- examples:
  - `makeSinOscEnv` -- oscillator + envelope + VCA, triggered by a single input
  - `makeTwoWtOp` -- two-operator FM synth (carrier + modulator)
  - `makeTwoWtOpTwoFreqEnv` -- two-operator FM with frequency envelopes on both operators
  - `makeWhiteNoiseOp` -- white noise + envelope

- the batch `connect(vector<int>)` syntax is used heavily here:
  ```cpp
  m->connect(std::vector<int>{
      MANAGER, 0,    s_trig,  0,     // manager input 0 -> trigger split
      s_trig,  0,    car,     0,     // trigger -> carrier
      s_trig,  1,    mod,     0,     // trigger -> modulator
      mod,     0,    car,     1,     // mod output -> carrier phase mod input
      car,     0,    MANAGER, 0      // carrier output -> manager output 0
  });
  ```

# advanced ugens

- in the `advanced/` directory, there are "all-in-one" ugens that combine multiple operations into a single `run()` loop
  - e.g. `AHRExpEnvVca` combines an envelope and a VCA in one ugen, avoiding the overhead of separate ugens and buffer reads/writes
  - `WavetableOp` combines a wavetable oscillator with an envelope
- these exist for performance and convenience -- less buffer copying, fewer virtual calls

# wavetable_utils.hpp

- contains functions to generate wavetables at startup
- `makeSinWavetable`, `makeSawWavetable`, `makeSquareWavetable`, `makeTriangleWavetable`, `makeWhiteNoiseWavetable`, `makeTanhWavetable`, `makeBitcrushWavetable`
- each fills a `vector<float>` with one cycle of the waveform
- also has `makeAHRWavetable` which generates an envelope shape as a wavetable

# memory model

- ugens are heap-allocated with `new` and never freed
- this is by design -- each "song" is a code file that creates ugens at startup
- the graph is built once and runs until the program exits
- the OS reclaims all memory on exit
- `BufferAllocator` is a bump allocator -- allocations grow forward, nothing is individually freed
