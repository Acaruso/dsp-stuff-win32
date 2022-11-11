- todo:

- add ability to delete connections and ugens
- instead of using `new` and `free` to allocate/deallocate ugen memory, create some sort of allocator
  - could have a vector for each type of ugen
  - could also create a union which wraps all types of ugens, then create a vector of these unions
  - could try to get rid of runtime polymorphism (virtual functions)
- instead of having each ugen own its input buffers, pass buffers around
  - recall that buffers are just int offsets now
- maybe we can stop using ids to identify ugens -- just use pointers
  - why exactly do we need ids?
- do "floating point denormalization"
  - see: https://www.earlevel.com/main/2019/04/19/floating-point-denormals/
- look into instinsics
- try using raw arrays rather than vectors
- figure out how to get rid of outputBuffers in ugen_manager
- dsp todo/ideas:
  - oscillator stuff:
    - multiply two sines
    - construct a complex signal (not a sine) somehow
      - fm synthesis, additive synthesis, etc
      - multiply complex signal by a sine
    - sin(t)^2, sin(t)^3, etc
    - sin(t)^1/2, sin(t)^1/4
- add markers to wave display -- to start with maybe just 1/4, 1/2, 3/4
- bitcrush + other bit effects
- in UI, instead of locking ugens, updating, and then unlocking, create lambdas or function pointers that do whatever ugen updates are desired
  - send these to the the audio thread via the lock-free queue
  - execute them on the audio thread
- sequencer stuff:
  - hi hats
  - multiple patterns
    - copy/paste patterns
  - default value per-track?
  - be able to trigger a step every x pattern repetitions
    - could also create a general "conditional trigger" lambda which could read a counter on the sequencer and use it to trigger or not
  - ugen that converts any non-zero data to 1.0f
  - multi-valued sequencer
    - each step can output multiple values
    - could use one value as trigger, another as freq
- idea: start with a UI that you wish you had, then implement functionality
- make a corresponding UI elt for each ugen
  - UI elt will give controls for all relevant parameters
- "matrix mixer" ugen
- instead of trying to write reusable, generic stuff, create something very specific
  - come up with an idea, write it down, implement it, and move on. assume that code will probably not be reused
  - example of an idea:
    - have a simple 808 style kick. have a slightly higher pitched synth playing a melody or bassline. have the kick FM the synth. instead of snares and hats, have metallic fm perc sounds. push rhythem behind the beat on the 2 and 4. randomize rhythems a bit. use bitwise operations on seq data (xor etc)
      - also try delaying the 8th note that hits right before 2 and 4
- instead of using sequencer to sequence individual notes/hits, create "events" at code level
  - events are more complex than single hits
  - use sequencer to sequence these events
  - general idea: individual hits are too "low level"
- create sequencer that can place events at a more granular level than 16th notes
  - should it be down to the sample level? or just something like 96 ppq?
  - could use something more like a hashtable
    - each sample, look in hashtable to see if there's an event
    - probably don't want to use unordered_map 
      - maybe implement our own hash table
        - avoid dynamic allocations
          - allocate a fixed size buffer, use this for all HT-related stuff
        - can maybe use **run-length encoding** (see designing data intensive applications notes)
          - typical sequencer:
            ```
            0 1 2 3 4 5 6 7
            1 0 0 0 1 0 1 0
            ```
          - we can store it like:
            ```
            0 1 2 3 4 5 6 7
            0 1 3 1 1 1
            ```
          - seems like inserting and deleting will be very complicated
        - actually, maybe we can just use a sorted array
          - at startup time, allocate some big array -- typically won't use all of it
          - event has structure `{ sample: 152235, other data... }`
          - sort events by sample
          - at each sample tick, `while (sampleCounter == events[i].sample)`, trigger `events[i]`, do `i++`
          - downside: when inserting or deleting events, need to re-sort
            - note that we don't need to do a full sort
              - if inserting: find correct place to insert, move everything over
              - if deleting: move everything over

# performance measurements

- performance measurements, 4 ugens:
  - new:
    - 2.38607
    - 2.85452
    - 3.09939
    - 2.09208
    - 2.12466
    - 2.16459
  - old:
    - 3.14234
    - 3.5898
    - 3.5429



calculating square and sqrt:
0.107137
0.107623

using wavetable:
0.0971675
0.106718




0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
k       k       k         k            
        s                 s




- from KVR:
  - lookup tables (ie wavetables, waveshapers) are expensive due to memory access
  - thus, it may be more efficient to use approx. computations rather than lookup tables

- exp envelope:
  - state += coefficient * (targetValue - state)














