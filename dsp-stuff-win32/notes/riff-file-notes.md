# riff files

- url: https://learn.microsoft.com/en-us/windows/win32/multimedia/resource-interchange-file-format-services



- riff files use 4-character codes called `FOURCC` codes to identify various **file elements**
  - each code is 32 bits in size
    - a char is 8 bits so `4 * 8 = 32`
  - each code is right-padded with space characters if necessary
  - the `mmioFOURCC()` macro can be used to create `FOURCC` codes

- a **chunk** is the basic building block of a riff file
  - each chunk is one "logical unit" of data
    - for example, one frame of a video
  - each chunk contains various **fields**:
    - **id**
      - a four character code which specifies the chunk identifier
    - **size**
      - a double word value specifying the size of the data member
    - **data**
      - "the data member" -- contains the media data

- a chunk can also contain **subchunks**
  - a chunk that contains subchunk(s) is called a **parent chunk**
  - only RIFF or LIST chunks can be parent chunks
  - the first chunk in a riff file is always a RIFF chunk
    - all other chunks in the file are subchunks of the RIFF chunk

- RIFF chunks have another field contained within the data field -- the **form type** field
  - the form type field is contained within the first four bytes of the data field
  - the form type field is a four character code that identifies the format type of the data
    - for example, wave files have the form type "WAVE"

- `mmioDescend()`
  - can use this to "descend" into a chunk
    - descending into a chunk == reading a chunk
  - after descending into a chunk, the file position pointer points to the data field of the chunk
  - after descending into a RIFF chunk, the file position pointer points to the location following the form type
    - ie, the location of the first data chunk



```cpp
parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

// parentChunkInfo partially specifies what we're searching for -- in this case, the WAVE header
// then, other parts of parentChunkInfo are filled in
// so parentChunkInfo is used to specify what to search for, and also used for storing the data that' found

auto mmioDescendRes = mmioDescend(
    wave->hMmio,
    (LPMMCKINFO)&parentChunkInfo,
    NULL,
    MMIO_FINDRIFF
);

// at this point, parentChunkInfo contains data re: the WAVE header

// now we find the fmt chunk

subchunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');

auto mmioDescendRes2 = mmioDescend(
    wave->hMmio,
    &subchunkInfo,
    &parentChunkInfo,
    MMIO_FINDCHUNK
);

// subchunkInfo.cksize now contains the chunkSize

// - now that we know the chunk size, we can read the actual chunk 
//   - previously, we just read the header
// - read fmt chunk into waveFormat which has type WAVEFORMATEX

auto mmioReadRes = mmioRead(
    wave->hMmio,                            // file handle
    (HPSTR)&wave->waveFormat,               // destination to read data into
    subchunkInfo.cksize                     // number of bytes to read
);

// in order to read another chunk, first need to "ascend" out of the chunk you just read

mmioAscend(wave->hMmio, &subchunkInfo, 0);
```













































