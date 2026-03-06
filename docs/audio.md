# audio_main.cpp

- `audioMain` is the entrypoint of the audio thread
- this is kicked off in `app.hpp:App::init`
  - `audioThread = std::thread(&audioMain, &sharedData);`
- `audioMain` creates `wasapiClient`
- it then creates `AudioService{wasapiClient, sharedData}`

# wasapi_client

- this is the lowest level audio code. it handles communicating to the audio interface using WASAPI (windows audio API)

- `hEvent`
  - this is a windows event object - a simple synchronization primitive which the main audio loop sleeps on (see below)
  - `WasapiClient::initEvent` initializes `hEvent`
    - `audioClient->SetEventHandle(hEvent);`
      - this tells `audioClient` to use `hEvent` to signal when it needs more audio data
  - `AudioService::run` contains the main audio loop
    - the main audio loop calls `WaitForSingleObject(wasapiClient.hEvent, INFINITE)`
      - this sleeps until `audioClient` signals on `hEvent`

- the audio buffer
  - within the audio hardware, there is a **buffer** of samples
  - this buffer stores some amount of audio data
  - audio data in the buffer is stored as **frames**
    - a frame is 2 samples if you're using stereo, or more if you're using surround sound etc.
    - usually audio data is stored as an array of samples. the frame is just an implicit organization of this array.
      - for example, given an array of samples `arr`, the first frame is `arr[0]` and `arr[1]`, the second is `arr[2]` and `arr[3]`, etc.
  - the **sample rate** is the number of frames (not samples) per second
    - for example, at 48k sample rate, there are 48k frames per second
      - if the buffer size is 48k frames, then it stores one second of audio
  - the audio thread sits in an infinite loop
    - fill audio buffer -> go to sleep -> wake up -> fill audio buffer -> ...
  - the audio buffer size is chosen by WASAPI at startup time

- `cacheBufferSizes`
  - this determines the audio buffer size as number of frames and also as number of bytes
    - these values are cached for later reuse
  - `audioClient->GetBufferSize` gets the buffer size in frames

- `getCurrentPadding`
  - the audio thread is usually not woken up on a "clean" boundary, i.e. it doesn't need to fill the entire sample buffer each time
  - instead, it only needs to fill part of the sample buffer
    - some of the audio data in the buffer has already been played by the hardware - we should overwrite this
    - some of the audio data in the buffer has not been played by the hardware - we shouldn't overwrite this
      - this is the **padding** - the padding is the number of frames in the audio buffer that the audio thread doesn't need to fill
  - example: if the audio buffer is 480 frames, and there are currently 80 frames of padding, then we only need to fill 300 frames



