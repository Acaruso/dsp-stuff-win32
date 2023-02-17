- X monophonic additive synth
- X wavetable based waveshaper
- X make more wavetables
  - X saw
  - X square
  - X triange
  - X bitcrush
- X high precision sequencer -- up to 96th notes

- better way to set values in sequencer
- sequence that can do swing and off-grid rhythms
- some sort of "composable wavetable"?
- more ideas for advanced seq:
  - have only one clock, shared across all advanced seqs, passed in via dep injection
  - have various "step presets"
    - each preset has an integer id
    - can then create a pattern like: `{ 1, 0, 0, 0, 2, 0, 3, 0 }`
      - `0`, `1`, `2`, and `3` are step presets
        - for example, `2` could be a 16th note triplet roll or something
















































