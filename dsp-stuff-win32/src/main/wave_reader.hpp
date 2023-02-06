// from: https://cboard.cprogramming.com/windows-programming/72183-output-file-1200-1300-1220-1220-ect.html
// also see: http://www.borg.com/~jglatt/tech/mmio.htm

#pragma once

#define NOMINMAX

#include <algorithm>
#include <iostream>

#include <windows.h>
#pragma comment(lib, "winmm.lib")

// Definition for the HXWAVE handle type. This structure should
// be considered opaque.
typedef struct {
    WAVEFORMATEX WaveFormat;        // file format of the wave file
    HMMIO        hMmio;             // file handle to the wave file
    DWORD        cbDataChunk;       // size of the wave data chunk
} *HXWAVE;      // type HXWAVE is a pointer to the above (nameless) struct type

// Structure to contain a wave sample.
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
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
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
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
            return NULL;
        }

        // Tell Windows to locate a WAVE FileType chunk header somewhere in the file.
        // This marks the start of any embedded WAVE format within the file

        // mmioFOURCC is a macro that converts four characters into a "four character code" (?)

        parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

        // "descend" into a chunk of the RIFF file
        // RIFF is the file format that WAV files use (and other stuff)

        auto mmioDescendRes = mmioDescend(
            wave->hMmio,                    // file handle to open RIFF file
            (LPMMCKINFO)&parentChunkInfo,   // "pointer to a buffer that receives an MMCKINFO structure" (?)
                                            //   I guess this struct says what we're looking for
                                            //   in this case, "WAVE"
            0,                              // optional
            MMIO_FINDRIFF                   // search flag
        );                                  //   MMIO_FINDRIFF searches for a chunk w/ identifier "RIFF"
                                            //   and "specified form type" (?)

        if (mmioDescendRes != MMSYSERR_NOERROR) {
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
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
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
            return NULL;
        }

        // read the "fmt " chunk into our WAVEFORMATEX structure

        auto mmioReadRes = mmioRead(
            wave->hMmio,                    // file handle
            (HPSTR)&wave->WaveFormat,       // destination to read data into
            std::min(                            // number of bytes to read
                subchunkInfo.cksize,        // i guess this is what we read earlier
                (DWORD)sizeof(wave->WaveFormat)
            )
        );

        if (mmioReadRes != std::min(subchunkInfo.cksize, (DWORD)sizeof(wave->WaveFormat))) {
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
            return NULL;
        }

        // check that the format is supported
        // todo: update this to allow 24 bit, etc.

        if (
            wave->WaveFormat.wFormatTag != WAVE_FORMAT_PCM
            || (wave->WaveFormat.wBitsPerSample != 16 && wave->WaveFormat.wBitsPerSample != 8)
            || (wave->WaveFormat.nChannels != 1 && wave->WaveFormat.nChannels != 2)
        ) {
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
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
            if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
            if (wave) HeapFree(GetProcessHeap(), 0, wave);
            return NULL;
        }

        // store the size of the data chunk (ie, the size of the waveform data)

        wave->cbDataChunk = subchunkInfo.cksize;

        return wave;

    // error:
    //     if (wave && wave->hMmio) mmioClose(wave->hMmio, 0);
    //     if (wave) HeapFree(GetProcessHeap(), 0, wave);
    //     return NULL;
    }

    VOID close(HXWAVE hWave) {
        mmioClose(hWave->hMmio, 0);
        HeapFree(GetProcessHeap(), 0, hWave);
    }

    VOID getFormat(HXWAVE hWave, WAVEFORMATEX* pFormat) {
        CopyMemory(
            pFormat,                // dest
            &hWave->WaveFormat,     // source
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

    LONG getNextSample(HXWAVE hWave, SAMPLE* pSample) {
        // read in left channel
        LONG ret = mmioRead(hWave->hMmio, (HPSTR) &pSample->left, hWave->WaveFormat.wBitsPerSample / 8);

        if (hWave->WaveFormat.nChannels == 2) {
            // read in right channel, if it exists
            ret = mmioRead(hWave->hMmio, (HPSTR) &pSample->right, hWave->WaveFormat.wBitsPerSample / 8);
        } else {
            // mono file - give the right sample the same value as the left sample
            pSample->right = pSample->left;
        }

        if (hWave->WaveFormat.wBitsPerSample == 8) {
            // normalize unsigned 8 bit samples to 16 bit unsigned
            pSample->left  *= 257;
            pSample->right *= 257;
        } else {
            // convert 16 bit signed samples to unsigned
            pSample->left  = (SHORT) pSample->left  + 32768;
            pSample->right = (SHORT) pSample->right + 32768;
        }

        return ret;
    }

    LONG getNextSampleFloat(HXWAVE hWave, float* pSample) {
        SAMPLE sample;

        LONG rc = getNextSample(hWave, &sample);

        (*pSample) = (((float)sample.left) * ushortToFloatRatio) - 1.0f;

        return rc;
    }
};

// example usage:

// int main(void) {
//     HXWAVE hWave = NULL;
//     SAMPLE sample = { 0 };

//     hWave = waveOpen(TEXT("C:\\WINDOWS\\MEDIA\\TADA.WAV"));

//     while (waveGetNextSample(hWave, &sample) > 0) {
//         printf("%u,%u\n", sample.left, sample.right);
//     }

//     waveClose(hWave);

//     return 0;
// }
