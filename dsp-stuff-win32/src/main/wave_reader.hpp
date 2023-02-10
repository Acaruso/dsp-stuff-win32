// from: https://cboard.cprogramming.com/windows-programming/72183-output-file-1200-1300-1220-1220-ect.html
// also see: https://web.archive.org/web/20080215200600/http://www.borg.com/~jglatt/tech/mmio.htm

#pragma once

#include <iostream>
#include <vector>

#include <windows.h>
#pragma comment(lib, "winmm.lib")

struct Wave {
    WAVEFORMATEX waveFormat;
    HMMIO fileHandle;
    DWORD waveSizeBytes;
    int sampleSizeBytes;
    int waveSizeSamples;
};

struct StereoSample {
    float left;
    float right;
};

// - TODO:
//   - handle upsampling
//     - if sample is 44.1k and current sample rate is 48k, upsample
//     - if sample is 48k and current sample rate is 44.1k, throw error
//   - add support for 24-bit samples

class WaveReader {
public:
    // scale range (-((1 << 15) - 1), ((1 << 15) - 1)) to range (-1.0f, 1.0f)
    float shortToFloatRatio = 1.0f / ((1 << 15) - 1);

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

        if (wave == nullptr) {
            close(wave);
            return nullptr;
        }

        // - open the file for reading with buffered I/O
        // - use default windows internal buffer
        //   - this could potentially be a problem because the windows internal buffer is only 8 KB
        //     - source: archive.org link at top of file

        wave->fileHandle = mmioOpen(
            (LPTSTR)fileName,
            nullptr,
            MMIO_READ | MMIO_ALLOCBUF | MMIO_DENYWRITE
        );

        if (wave->fileHandle == nullptr) {
            close(wave);
            return nullptr;
        }

        // check that the file is actually a WAV file, by finding a RIFF chunk with the "WAVE" form type

        // mmioFOURCC is a macro that converts four characters into a "four character code"

        // by setting parentChunkInfo.fccType to WAVE and then passing that to mmioDescend,
        // we tell mmioDescend to search for the first chunk with this header

        parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

        MMRESULT mmioDescendRes = mmioDescend(
            wave->fileHandle,
            (LPMMCKINFO)&parentChunkInfo,   // thing to search for
            nullptr,                        // optional
            MMIO_FINDRIFF                   // search flag
        );                                  //   MMIO_FINDRIFF == search for a chunk with the identifier "RIFF"
                                            //   and the form type specified in arg 2

        if (mmioDescendRes != MMSYSERR_NOERROR) {
            close(wave);
            return nullptr;
        }

        // locate the "fmt " chunk, and read its size field

        subchunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');

        MMRESULT mmioDescendRes2 = mmioDescend(
            wave->fileHandle,
            &subchunkInfo,                  // thing to search for
                                            //   also destination to read data into
            &parentChunkInfo,               // extra info about parent
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes2 != MMSYSERR_NOERROR) {
            close(wave);
            return nullptr;
        }

        // subchunkInfo.cksize now contains the size of the fmt chunk

        // read the "fmt " chunk into wave->waveFormat

        // note that mmioDescend() reads header data, while mmioRead() reads chunk data

        LONG mmioReadRes = mmioRead(
            wave->fileHandle,
            (HPSTR)&wave->waveFormat,               // destination
            subchunkInfo.cksize                     // number of bytes to read
        );

        if (mmioReadRes != subchunkInfo.cksize) {
            close(wave);
            return nullptr;
        }

        // check that the format is supported
        // TODO: update this to allow 24 bit, etc.

        if (
            wave->waveFormat.wFormatTag != WAVE_FORMAT_PCM
            || (wave->waveFormat.wBitsPerSample != 16)
            // || (wave->waveFormat.wBitsPerSample != 16 && wave->waveFormat.wBitsPerSample != 24)
            || (wave->waveFormat.nChannels != 1 && wave->waveFormat.nChannels != 2)
        ) {
            close(wave);
            return nullptr;
        }


        // ascend out of the fmt subchunk
        // you need to ascend out of any chunks that you've descended into, before you can read any other chunks

        mmioAscend(wave->fileHandle, &subchunkInfo, 0);

        // locate the data chunk

        subchunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');

        MMRESULT mmioDescendRes3 = mmioDescend(
            wave->fileHandle,
            &subchunkInfo,
            &parentChunkInfo,
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes3 != MMSYSERR_NOERROR)  {
            close(wave);
            return nullptr;
        }

        // wave->fileHandle now points to the waveform data within the data chunk

        wave->waveSizeBytes = subchunkInfo.cksize;
        wave->sampleSizeBytes = wave->waveFormat.wBitsPerSample / 8;

        // note that a sample in this case is not a stereo sample
        wave->waveSizeSamples = wave->waveSizeBytes / wave->sampleSizeBytes;

        return wave;
    }

    VOID close(Wave* wave) {
        if (wave && wave->fileHandle) mmioClose(wave->fileHandle, 0);
        if (wave) HeapFree(GetProcessHeap(), 0, wave);
    }

    int readNext16BitStereoSample(Wave* wave, StereoSample* sample) {
        int rc = 0;

        SHORT leftShort = 0;
        SHORT rightShort = 0;

        int numBytesToRead = wave->waveFormat.wBitsPerSample / 8;

        rc = mmioRead(
            wave->fileHandle,
            (HPSTR)(&leftShort),
            numBytesToRead
        );

        rc = mmioRead(
            wave->fileHandle,
            (HPSTR)(&rightShort),
            numBytesToRead
        );

        sample->left  = shortToFloat(leftShort);
        sample->right = shortToFloat(rightShort);

        return rc;
    }

    int readNext16BitMonoSample(Wave* wave, float* f_sample) {
        int rc = 0;
        SHORT s_sample = 0;

        int numBytesToRead = wave->waveFormat.wBitsPerSample / 8;

        rc = mmioRead(
            wave->fileHandle,
            (HPSTR)(&s_sample),
            numBytesToRead
        );

        (*f_sample) = shortToFloat(s_sample);

        return rc;
    }

    void fillWave(Wave* wave, std::vector<float>* waveVec) {
        if (wave->waveFormat.nChannels == 1) {
            // TODO: test this
            float f_sample;
            for (int i = 0; i < wave->waveSizeSamples; ++i) {
                readNext16BitMonoSample(wave, &f_sample);
                waveVec->push_back(f_sample);
            }
        } else if (wave->waveFormat.nChannels == 2) {
            StereoSample sample;
            for (int i = 0; i < (wave->waveSizeSamples / 2); ++i) {
                readNext16BitStereoSample(wave, &sample);
                waveVec->push_back(sample.left);
            }
        }
    }

    // convert 16-bit signed int to float in range (-1.0f, 1.0f)
    float shortToFloat(SHORT s) {
        return ((float)s) * shortToFloatRatio;
    }
};
