- some sequencers are contained in `src\audio\ugens\seqs`
  - these are mostly "simple" in their timing method and resolution -- they use a simple fixed 16th note grid, similar to a drum machine sequencer
  - `sample_seq.hpp` is more complex though -- it allows you to place events at sample resolution
    - however, this also means that you can't easily change the tempo, and that the tempo is tied to the current sample rate

- **song2** actually implements its own sequencers and it mostly doesn't use the ugen system
  - these sequencers are more advanced and better than the sequencers in `src\audio\ugens\seqs`
  - the best sequencer to look at is `src\audio\ugens\song2\song2_seq.hpp:AdvancedSeq`

- `AdvancedSeq`
  - instead of placing events at sample granularity, `AdvancedSeq` places them at "pulse" granularity, using a 96 pulse-per-quarter-note (PPQ) resolution
  - this allows it to place events with fairly high resolution without being tied to sample rate, and with the ability to change the tempo
  - events are stored in a big vector, `vector<AdvancedSeqEvent>`
  - each `AdvancedSeqEvent` has an member variable, `pos`
  - `events` is stored in sorted order, sorted on `pos`
  - when a new event is added to `events`, it is re-sorted on `pos`

- `src\audio\ugens\song2\song2_seq.hpp:SeqClock` is the "sequencer clock"
  - there is one `SeqClock` object which is shared by all sequencer objects
  - `SeqClock` contains various member variables:
    - `sTo384`
      - each sample, `sTo384` is incremented
      - once `sTo384` reaches `samplesPer384thNote`, it resets to `0`
      - `is384Note` returns `true` if `sTo384 == 0`
        - this is called in `AdvancedSeq.trigger` (see below)
    - `_384ToM`
      - increments by 1 every 384th note
      - resets at the end of each measure
    - note how the naming works:
      - syntax: `XToY`
      - `X` is the counter
      - `Y` is the modulus
      - `sTo384` is incremented every sample, and rolls over to `0` every 384th note
      - `_384ToM` is incremented every 384th note, and rolls over to `0` every measure

- how `SeqClock` is used in `AdvancedSeq`:
  - `AdvancedSeq.trigger` is called every sample
  - `AdvancedSeq.trigger` calls `seqClock->is384Note()`
    - this checks if the sample counter is currently on a 384th note boundary
  - if `seqClock->is384Note() == true`, it then checks if there is any event with `pos == seqClock->_384ToM`
  - thus, we use `sTo384` as sort of a higher-level check
    - we use `sTo384` to check whether we need to check if there's an event
    - if so, we then use `_384ToM` to actually check if there's an event
  - note that each sequence can only be one measure long -- this is somewhat of a big limitation, I was probably planning to extend the sequencer to have longer sequences at some point





