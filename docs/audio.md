# audio_main.cpp

- `audioMain` is the entrypoint of the audio thread
- `audioMain` is kicked off by `app.hpp:App::init`
  - `audioThread = std::thread(&audioMain, &sharedData);`
- `audioMain` creates `wasapiClient`
- it then creates `AudioService{wasapiClient, sharedData}`
- it then calls `audioService.run()`

# wasapi_client.cpp

- `WasapiClient` is the lowest level audio code. it handles communicating to the audio interface using WASAPI (windows audio API)

- `WasapiClient` has a member variable `hEvent`
  - `hEvent` is a windows event object - a simple synchronization primitive which the main audio loop sleeps on (see below)
  - `WasapiClient::initEvent` initializes `hEvent` and tells `audioClient` to use `hEvent` to signal when it needs more audio data
    - `audioClient->SetEventHandle(hEvent)`
  - the main audio loop (contained in `AudioService::run`) sleeps on `hEvent` until the hardware signals that it needs more audio data
    - to sleep, the main audio loop calls `WaitForSingleObject(wasapiClient.hEvent, INFINITE)`

- the audio buffer
  - within the audio hardware, there is a **buffer** of samples
  - this buffer stores some amount of audio data
  - audio data in the buffer is stored as **frames**
    - a frame consists of 2 samples if you're using stereo, or more if you're using surround sound etc.
    - usually audio data is stored as an array of samples. the frame is just an implicit organization of this array.
      - for example, given an array of samples `arr`, the first frame consists of `arr[0]` and `arr[1]`, with `arr[0]` containing the left channel's data and `arr[1]` containing the right channel's data
  - the **sample rate** is the number of frames (not samples) per second
  - example:
    - assume that we're using 48k sample rate, stereo
    - there are 48k frames per second
    - if the buffer size is 48k frames, then it stores one second of audio
    - the buffer contains 48k * 2 samples
  - the audio thread sits in an infinite loop
    - fill audio buffer -> go to sleep -> wake up -> fill audio buffer -> ...
  - the audio buffer size is chosen by WASAPI at startup time

- `WasapiClient::cacheBufferSizes`
  - this gets the audio buffer size as number of frames and also as number of bytes
    - these values are cached for later reuse
  - `audioClient->GetBufferSize` gets the buffer size in frames

- `WasapiClient::getCurrentPadding`
  - the audio thread is usually not woken up on a "clean" boundary, i.e. it doesn't need to fill the entire sample buffer each time its woken up
  - instead, audio thread only needs to fill part of the sample buffer
    - some of the audio data in the buffer has already been played by the hardware - we should overwrite this
    - some of the audio data in the buffer has not been played by the hardware - we shouldn't overwrite this
      - this is the **padding** - the padding is the number of frames in the audio buffer that the audio thread doesn't need to fill
  - example: if the audio buffer is 480 frames, and there are currently 80 frames of padding, then we only need to fill 400 frames

- `WasapiClient::writeBuffer`
  - called by `AudioService` to write data to the audio buffer
  - this audio data will then get played out by the audio hardware

# audio_service.cpp and sample_maker.hpp

- `AudioService::run`
  - recall that `audioMain` calls `AudioService::run`
  - `run` contains the main audio loop, which sleeps on `wasapiClient.hEvent` until its time to write more audio data, at which point it wakes up
  - how `run` fills the sample buffer:
    - `run` calls `wasapiClient.getCurrentPadding` to get the current padding
    - it uses this to compute the number of samples to write
    - it calls `AudioService::fillSampleBuffer(numSamplesToWrite)`
    - fillSampleBuffer calls `SampleMaker::makeSamples`

- `SampleMaker::makeSamples`
  - `SampleMaker` contains a member variable `UgenManager* root`
    - `root` is the root ugen of the entire project
    - where does `root` come from?
      - `main.cpp:wWinMain` creates `App` as a stack variable
      - `App` contains `SharedData sharedData`
      - `SharedData` contains `UgenManager rootUgen`
      - `app.hpp:App::init` adds the `outSum` and `outSink` ugens to `root`
  - `AudioService` has a member variable, `sampleCounter`
    - `sampleCounter` is an always-increasing count of the number of samples that have elapsed since the program started
    - `sampleCounter` can useful for certain ugens like sequencers
      - however, most DSP ugens don't use it. they contain their own accumulators that they use to "drive" their DSP.
    - `AudioService::fillSampleBuffer` increments `sampleCounter`
      - note that this happens in `AudioService`, not `SampleMaker`
    - `AudioService::fillSampleBuffer` passes `sampleCounter` into `SampleMaker::makeSamples`
    - `SampleMaker::makeSamples` passes `sampleCounter` into `root->run` (see below)
  - `makeSamples` calls `root->run(sampleCounter)` to run the ugen graph
    - the output of the ugen graph is ultimately stored in `outSink->buffer`
  - `makeSamples` returns `outSink->buffer` by reference
  - `makeSamples` also locks `rootUgenLock` before beginning to run the ugen graph, and unlocks it after

- `AudioService::fillSampleBuffer`
  - calls `SampleMaker::makeSamples` to get the vector of samples from the ugen graph
  - loops over this vector
    - this loop converts monophonic `float` samples into stereo `unsigned` 24-bit samples and sends them to the hardware
    - `scaleSignal` converts a `float` sample to a 24-bit `unsigned` sample
      - the hardware expects the leftmost 24 bits of each 32-bit `unsigned` to contain the audio data. the rightmost 8 bits should be `0`.
    - the loop writes samples into `AudioService::sampleBuffer`
      - `AudioService::run` then writes `AudioService::sampleBuffer` to the audio hardware by calling `wasapiClient.writeBuffer(sampleBuffer.buffer, numFramesToWrite)`





