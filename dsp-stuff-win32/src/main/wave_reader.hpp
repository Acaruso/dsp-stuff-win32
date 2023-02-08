// from: https://cboard.cprogramming.com/windows-programming/72183-output-file-1200-1300-1220-1220-ect.html
// also see: https://web.archive.org/web/20080215200600/http://www.borg.com/~jglatt/tech/mmio.htm

#pragma once

#define NOMINMAX

#include <algorithm>
#include <iostream>
#include <vector>

#include <windows.h>
#pragma comment(lib, "winmm.lib")

struct Wave {
    WAVEFORMATEX waveFormat;        // file format of the wave file
    HMMIO        fileHandle;        // file handle to the wave file
    DWORD        cbDataChunk;       // size of the wave data chunk
};

struct SAMPLE {
    USHORT left;        // USHORT is a 16 bit unsigned int
    USHORT right;
};

struct FloatSample {
    float left;
    float right;
};

// - TODO:
//   - handle upsampling
//     - if sample is 44.1k and current sample rate is 48k, upsample
//     - if sample is 48k and current sample rate is 44.1k, throw error
//   - add support for 24-bit samples
//   - remove support for 8-bit samples

class WaveReader {
public:
    // open the wave file. the format must be one of the following:
    //     "PCM, 16 bit stereo"
    //     "PCM, 8  bit stereo"
    //     "PCM, 16 bit mono"
    //     "PCM, 8  bit mono"

    // scale range (0, (1 << 16) - 1) to range (0.0f, 2.0f)
    float ushortToFloatRatio = 2.0f / ((1 << 16) - 1);

    // MMCKINFO struct:

    // struct MMCKINFO {
    //     FOURCC ckid;             // chunk id
    //     DWORD  cksize;           // size in bytes of the data member of the chunk
    //                              // size does not include:
    //                              //   the 4-byte chunk identifier
    //                              //   the 4-byte chunk size
    //                              //   the optional pad byte at the end of the data member
    //     FOURCC fccType;          // form type (?)
    //     DWORD  dwDataOffset;     // offset of the beginning of the chunk's data member, relative to the beginning of the file.
    //     DWORD  dwFlags;          // flags -- we don't use
    // };

    Wave* open(LPCTSTR fileName) {
        MMCKINFO parentChunkInfo;
        MMCKINFO subchunkInfo;
        Wave* wave;

        // HeapAlloc() is like malloc()

        wave = (Wave*)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(Wave)
        );

        if (wave == NULL) {
            close(wave);
            return NULL;
        }

        // - open the file for reading with buffered I/O
        // - use default windows internal buffer
        //   - this could potentially be a problem because the windows internal buffer is only 8 KB
        //     - source: archive.org link at top of file

        wave->fileHandle = mmioOpen(
            (LPTSTR)fileName,
            NULL,
            MMIO_READ | MMIO_ALLOCBUF | MMIO_DENYWRITE
        );

        if (wave->fileHandle == NULL) {
            close(wave);
            return NULL;
        }

        // check that the file is actually a WAV file, by finding a RIFF chunk with the "WAVE" form type

        // mmioFOURCC is a macro that converts four characters into a "four character code"

        // by setting parentChunkInfo.fccType to WAVE and then passing that to mmioDescend,
        // we tell mmioDescend to search for the first chunk with this header

        parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

        auto mmioDescendRes = mmioDescend(
            wave->fileHandle,
            (LPMMCKINFO)&parentChunkInfo,   // thing to search for
            NULL,                           // optional
            MMIO_FINDRIFF                   // search flag
        );                                  //   MMIO_FINDRIFF == search for a chunk with the identifier "RIFF"
                                            //   and the form type specified in arg 2

        if (mmioDescendRes != MMSYSERR_NOERROR) {
            close(wave);
            return NULL;
        }

        // locate the "fmt " chunk, and read its size field

        subchunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');

        auto mmioDescendRes2 = mmioDescend(
            wave->fileHandle,
            &subchunkInfo,                  // thing to search for
                                            //   also destination to read data into
            &parentChunkInfo,               // extra info about parent
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes2 != MMSYSERR_NOERROR) {
            close(wave);
            return NULL;
        }

        // subchunkInfo.cksize now contains the size of the fmt chunk

        // read the "fmt " chunk into wave->waveFormat

        // auto mmioReadRes = mmioRead(
        //     wave->fileHandle,
        //     (HPSTR)&wave->waveFormat,               // destination
        //     std::min(                               // number of bytes to read
        //         subchunkInfo.cksize,
        //         (DWORD)sizeof(WAVEFORMATEX)
        //     )
        // );

        auto mmioReadRes = mmioRead(
            wave->fileHandle,
            (HPSTR)&wave->waveFormat,               // destination
            subchunkInfo.cksize                     // number of bytes to read
        );

        // if (mmioReadRes != std::min(subchunkInfo.cksize, (DWORD)sizeof(wave->waveFormat))) {
        //     close(wave);
        //     return NULL;
        // }

        if (mmioReadRes != subchunkInfo.cksize) {
            close(wave);
            return NULL;
        }

        // check that the format is supported
        // TODO: update this to allow 24 bit, etc.

        if (
            wave->waveFormat.wFormatTag != WAVE_FORMAT_PCM
            || (wave->waveFormat.wBitsPerSample != 16 && wave->waveFormat.wBitsPerSample != 8)
            || (wave->waveFormat.nChannels != 1 && wave->waveFormat.nChannels != 2)
        ) {
            close(wave);
            return NULL;
        }

        // ascend out of the fmt subchunk
        // you need to ascend out of any chunks that you've descended into, before you can read any other chunks

        mmioAscend(wave->fileHandle, &subchunkInfo, 0);

        // locate the data chunk

        subchunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');

        auto mmioDescendRes3 = mmioDescend(
            wave->fileHandle,
            &subchunkInfo,
            &parentChunkInfo,
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes3 != MMSYSERR_NOERROR)  {
            close(wave);
            return NULL;
        }

        // wave->fileHandle now points to the waveform data within the data chunk

        // store the size of the data chunk (ie, the size of the waveform data)
        wave->cbDataChunk = subchunkInfo.cksize;

        return wave;
    }

    VOID close(Wave* wave) {
        if (wave && wave->fileHandle) mmioClose(wave->fileHandle, 0);
        if (wave) HeapFree(GetProcessHeap(), 0, wave);
    }

    VOID getFormat(Wave* wave, WAVEFORMATEX* format) {
        CopyMemory(
            format,                 // dest
            &(wave->waveFormat),    // source
            sizeof(*format)         // size
        );
    }

    // read a sample into the structure pointed to by pSample
    // all sample values are normalized to 16bit stereo
    // return Values:
    //     a return value greater than 0 indicates success.
    //     a return value of 0 indicates end of file.
    //     a return value of less than 0 indicates error.

    // LONG is a 32-bit signed integer

    LONG getNextSample(Wave* wave, SAMPLE* sample) {
        // read in left channel
        // mmioRead() returns 0 if it reaches the end of the file
        LONG ret = mmioRead(
            wave->fileHandle,                           // file handle
            (HPSTR)&sample->left,                   // destination
            wave->waveFormat.wBitsPerSample / 8    // number of bytes to read
        );

        if (wave->waveFormat.nChannels == 2) {
            // read in right channel, if it exists
            ret = mmioRead(
                wave->fileHandle,
                (HPSTR)&sample->right,
                wave->waveFormat.wBitsPerSample / 8
            );
        } else {
            // mono file - give the right sample the same value as the left sample
            sample->right = sample->left;
        }

        if (wave->waveFormat.wBitsPerSample == 8) {
            // convert unsigned 8-bit samples to 16-bit unsigned samples
            sample->left *= 257;
            sample->right *= 257;
        } else if (wave->waveFormat.wBitsPerSample == 16) {
            // convert 16-bit signed samples to 16-bit unsigned samples
            sample->left = ((SHORT)sample->left) + 32768;
            sample->right = ((SHORT)sample->right) + 32768;
        }

        return ret;
    }

    LONG getNextSampleFloat(Wave* wave, float* fSample) {
        SAMPLE sample;

        LONG rc = getNextSample(wave, &sample);

        (*fSample) = (((float)sample.left) * ushortToFloatRatio) - 1.0f;

        return rc;
    }

    void fillWave(Wave* wave, std::vector<float>* waveVec) {
        int i = 0;
        float fSample;
        // getNextSampleFloat() returns 0 if we reach the end of the file
        // wave->cbDataChunk is number of samples in file
        // however, samples are stereo and we're just using mono, so divide by 2
        while (getNextSampleFloat(wave, &fSample) && i < (wave->cbDataChunk / 2)) {
            waveVec->push_back(fSample);
            ++i;
        }
    }
};
