// from: https://cboard.cprogramming.com/windows-programming/72183-output-file-1200-1300-1220-1220-ect.html
// also see: https://web.archive.org/web/20080215200600/http://www.borg.com/~jglatt/tech/mmio.htm

#pragma once

#define NOMINMAX

#include <algorithm>
#include <iostream>
#include <vector>

#include <windows.h>
#pragma comment(lib, "winmm.lib")

// Definition for the HXWAVE handle type. This structure should
// be considered opaque.
typedef struct {
    WAVEFORMATEX waveFormat;        // file format of the wave file
    HMMIO        hMmio;             // file handle to the wave file
    DWORD        cbDataChunk;       // size of the wave data chunk
} *HXWAVE;      // type HXWAVE is a pointer to the above (nameless) struct type

typedef struct {
    USHORT left;        // USHORT is a 16 bit unsigned int
    USHORT right;
} SAMPLE;

struct FloatSample {
    float left;
    float right;
};

// - TODO:
//   - convert data to float
//   - handle upsampling
//     - if sample is 44.1k and current sample rate is 48k, upsample
//     - if sample is 48k and current sample rate is 44.1k, throw error
//   - handle 24 bit samples

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

    HXWAVE open(LPCTSTR fileName) {
        MMCKINFO parentChunkInfo;       // parent chunk information structure
        MMCKINFO subchunkInfo;          // subchunk information structure
        HXWAVE wave;                    // HXWAVE data structure

        // allocate memory for the wave handle, store pointer in `wave`
        // HeapAlloc() is like malloc()

        wave = (HXWAVE)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(*wave)
        );
        if (wave == NULL) {
            close(wave);
            return NULL;
        }

        // open the file for reading with buffered I/O
        // use default windows internal buffer (?)
        // documentation says: "This function is deprecated. Applications should call CreateFile to create or open files."
        //     https://learn.microsoft.com/en-us/windows/win32/api/mmiscapi/nf-mmiscapi-mmioopen
        // returns a file handle which references the opened file
        // file handle has type HMMIO
        // store file handle in `wave->hMmio`

        wave->hMmio = mmioOpen(
            (LPTSTR)fileName,
            NULL,
            MMIO_READ | MMIO_ALLOCBUF | MMIO_DENYWRITE
        );
        if (wave->hMmio == NULL) {
            close(wave);
            return NULL;
        }

        // locate a "RIFF" chunk with a "WAVE" form type
        // this is to check that the file is actually a WAV file

        // mmioFOURCC is a macro that converts four characters into a "four character code" (?)

        // by setting parentChunkInfo.fccType to WAVE and then passing that to mmioDescend
        // we tell mmioDescend to find the first chunk with this header

        parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

        auto mmioDescendRes = mmioDescend(
            wave->hMmio,                    // file handle to open RIFF file
            (LPMMCKINFO)&parentChunkInfo,   // what we're searching for
            NULL,                           // optional
            MMIO_FINDRIFF                   // search flag
        );                                  //   MMIO_FINDRIFF searches for a chunk w/ identifier "RIFF"
                                            //   and "specified form type" (?)

        if (mmioDescendRes != MMSYSERR_NOERROR) {
            close(wave);
            return NULL;
        }

        // locate the WAV file's "fmt " chunk, and read its size field

        subchunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');

        auto mmioDescendRes2 = mmioDescend(
            wave->hMmio,
            &subchunkInfo,
            &parentChunkInfo,
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes2 != MMSYSERR_NOERROR) {
            close(wave);
            return NULL;
        }

        // read the "fmt " chunk into our WAVEFORMATEX structure

        auto mmioReadRes = mmioRead(
            wave->hMmio,                    // file handle
            (HPSTR)&wave->waveFormat,       // destination to read data into
            std::min(                            // number of bytes to read
                subchunkInfo.cksize,        // i guess this is what we read earlier
                (DWORD)sizeof(wave->waveFormat)
            )
        );

        if (mmioReadRes != std::min(subchunkInfo.cksize, (DWORD)sizeof(wave->waveFormat))) {
            close(wave);
            return NULL;
        }

        // check that the format is supported
        // todo: update this to allow 24 bit, etc.

        if (
            wave->waveFormat.wFormatTag != WAVE_FORMAT_PCM
            || (wave->waveFormat.wBitsPerSample != 16 && wave->waveFormat.wBitsPerSample != 8)
            || (wave->waveFormat.nChannels != 1 && wave->waveFormat.nChannels != 2)
        ) {
            close(wave);
            return NULL;
        }

        // ascend out of the "fmt " subchunk
        // you need to ascend out of any chunks you've descended into, before you can read any other chunks

        mmioAscend(wave->hMmio, &subchunkInfo, 0);

        // locate the data chunk
        // upon return, the file pointer will be ready to read in the actual waveform data within
        // the data chunk

        subchunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');

        auto mmioDescendRes3 = mmioDescend(
            wave->hMmio,
            &subchunkInfo,
            &parentChunkInfo,
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes3 != MMSYSERR_NOERROR)  {
            close(wave);
            return NULL;
        }

        // store the size of the data chunk (ie, the size of the waveform data)

        wave->cbDataChunk = subchunkInfo.cksize;

        return wave;
    }

    VOID close(HXWAVE wave) {
        if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
        if (wave) HeapFree(GetProcessHeap(), 0, wave);
    }

    VOID getFormat(HXWAVE hWave, WAVEFORMATEX* pFormat) {
        CopyMemory(
            pFormat,                // dest
            &hWave->waveFormat,     // source
            sizeof(*pFormat)        // size
        );
    }

    // read a sample into the structure pointed to by pSample
    // all sample values are normalized to 16bit stereo
    // return Values:
    //     a return value greater than 0 indicates success.
    //     a return value of 0 indicates end of file.
    //     a return value of less than 0 indicates error.

    // LONG is a 32-bit signed integer

    LONG getNextSample(HXWAVE hWave, SAMPLE* sample) {
        // read in left channel
        // mmioRead() returns 0 if it reaches the end of the file
        LONG ret = mmioRead(
            hWave->hMmio,                           // file handle
            (HPSTR)&sample->left,                   // destination
            hWave->waveFormat.wBitsPerSample / 8    // number of bytes to read
        );

        if (hWave->waveFormat.nChannels == 2) {
            // read in right channel, if it exists
            ret = mmioRead(
                hWave->hMmio,
                (HPSTR)&sample->right,
                hWave->waveFormat.wBitsPerSample / 8
            );
        } else {
            // mono file - give the right sample the same value as the left sample
            sample->right = sample->left;
        }

        if (hWave->waveFormat.wBitsPerSample == 8) {
            // convert unsigned 8-bit samples to 16-bit unsigned samples
            sample->left *= 257;
            sample->right *= 257;
        } else if (hWave->waveFormat.wBitsPerSample == 16) {
            // convert 16-bit signed samples to 16-bit unsigned samples
            sample->left = ((SHORT)sample->left) + 32768;
            sample->right = ((SHORT)sample->right) + 32768;
        }

        return ret;
    }

    LONG getNextSampleFloat(HXWAVE hWave, float* pSample) {
        SAMPLE sample;

        LONG rc = getNextSample(hWave, &sample);

        (*pSample) = (((float)sample.left) * ushortToFloatRatio) - 1.0f;

        return rc;
    }

    void fillWave(HXWAVE hWave, std::vector<float>* wave) {
        int i = 0;
        float fSample;
        // getNextSampleFloat() returns 0 if we reach the end of the file
        // hWave->cbDataChunk is number of samples in file
        // however, samples are stereo and we're just using mono, so divide by 2
        while (getNextSampleFloat(hWave, &fSample) && i < (hWave->cbDataChunk / 2)) {
        // while (getNextSampleFloat(hWave, &fSample)) {
            wave->push_back(fSample);
            ++i;
        }
        std::cout << "i: " << i << std::endl;
    }
};
