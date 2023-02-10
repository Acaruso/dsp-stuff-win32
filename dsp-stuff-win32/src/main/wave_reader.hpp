// from: https://cboard.cprogramming.com/windows-programming/72183-output-file-1200-1300-1220-1220-ect.html
// also see: https://web.archive.org/web/20080215200600/http://www.borg.com/~jglatt/tech/mmio.htm

#pragma once

#include <iostream>
#include <string>
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
    void openWaveFileAndFillFloatVec(std::wstring fileName, std::vector<float>* floatVec) {
        Wave* wave = openWaveFile(fileName);
        if (wave == nullptr) {
            return;
        }
        fillFloatVec(wave, floatVec);
        close(wave);
    }

    Wave* openWaveFile(std::wstring fileName) {
        std::string str(fileName.begin(), fileName.end());
        std::cout << "opening wave file: " << str << std::endl;

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
            closeWithError(wave);
            return nullptr;
        }

        // - open the file for reading with buffered I/O
        // - use default windows internal buffer
        //   - this could potentially be a problem because the windows internal buffer is only 8 KB
        //     - source: archive.org link at top of file

        wave->fileHandle = mmioOpen(
            (LPTSTR)fileName.c_str(),
            nullptr,
            MMIO_READ | MMIO_ALLOCBUF | MMIO_DENYWRITE
        );

        if (wave->fileHandle == nullptr) {
            closeWithError(wave);
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
            closeWithError(wave);
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
            closeWithError(wave);
            return nullptr;
        }

        // subchunkInfo.cksize now contains the size of the fmt chunk

        // read the "fmt " chunk into wave->waveFormat
        // note that mmioDescend() reads header data, while mmioRead() reads chunk data
        // mmioRead() returns the number of bytes read

        LONG mmioReadRes = mmioRead(
            wave->fileHandle,
            (HPSTR)&wave->waveFormat,               // destination
            subchunkInfo.cksize                     // number of bytes to read
        );

        if (mmioReadRes != subchunkInfo.cksize) {
            closeWithError(wave);
            return nullptr;
        }

        // check that the format is supported
        // TODO: update this to allow 24 bit, etc.

        std::cout << "sample rate: " << wave->waveFormat.nSamplesPerSec << std::endl;
        std::cout << "bits per sample: " << wave->waveFormat.wBitsPerSample << std::endl;

        if (
            wave->waveFormat.wFormatTag != WAVE_FORMAT_PCM
            || (wave->waveFormat.wBitsPerSample != 16)
            // || (wave->waveFormat.wBitsPerSample != 16 && wave->waveFormat.wBitsPerSample != 24)
            || (wave->waveFormat.nChannels != 1 && wave->waveFormat.nChannels != 2)
        ) {
            closeWithError(wave);
            return nullptr;
        }

        // ascend out of the fmt subchunk
        // you need to ascend out of any chunks that you've descended into, before you can read any other chunks

        mmioAscend(wave->fileHandle, &subchunkInfo, 0);

        // find the data chunk

        subchunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');

        MMRESULT mmioDescendRes3 = mmioDescend(
            wave->fileHandle,
            &subchunkInfo,
            &parentChunkInfo,
            MMIO_FINDCHUNK
        );

        if (mmioDescendRes3 != MMSYSERR_NOERROR)  {
            closeWithError(wave);
            return nullptr;
        }

        // wave->fileHandle now points to the waveform data within the data chunk

        // set wave size stuff

        wave->waveSizeBytes = subchunkInfo.cksize;
        wave->sampleSizeBytes = wave->waveFormat.wBitsPerSample / 8;

        // note that a "sample" in this case is a mono sample, not a stereo sample
        wave->waveSizeSamples = wave->waveSizeBytes / wave->sampleSizeBytes;

        return wave;
    }

    void fillFloatVec(Wave* wave, std::vector<float>* floatVec) {
        bool rc = true;

        if (wave->waveFormat.nChannels == 1) {
            float f_sample;
            for (int i = 0; i < wave->waveSizeSamples; ++i) {
                rc = readNext16BitMonoSample(wave, &f_sample);
                if (!rc) {
                    closeWithError(wave);
                    return;
                }
                floatVec->push_back(f_sample);
            }
        } else if (wave->waveFormat.nChannels == 2) {
            StereoSample sample;
            for (int i = 0; i < (wave->waveSizeSamples / 2); ++i) {
                rc = readNext16BitStereoSample(wave, &sample);
                if (!rc) {
                    closeWithError(wave);
                    return;
                }
                floatVec->push_back(sample.left);
            }
        }
    }

    bool readNext16BitStereoSample(Wave* wave, StereoSample* sample) {
        int numBytesRead = 0;

        SHORT leftShort = 0;
        SHORT rightShort = 0;

        int numBytesToRead = wave->waveFormat.wBitsPerSample / 8;

        numBytesRead = mmioRead(
            wave->fileHandle,
            (HPSTR)(&leftShort),
            numBytesToRead
        );

        if (numBytesRead != numBytesToRead) {
            return false;
        }

        numBytesRead = mmioRead(
            wave->fileHandle,
            (HPSTR)(&rightShort),
            numBytesToRead
        );

        if (numBytesRead != numBytesToRead) {
            return false;
        }

        sample->left  = shortToFloat(leftShort);
        sample->right = shortToFloat(rightShort);

        return true;
    }

    bool readNext16BitMonoSample(Wave* wave, float* f_sample) {
        int numBytesRead = 0;
        SHORT s_sample = 0;

        int numBytesToRead = wave->waveFormat.wBitsPerSample / 8;

        numBytesRead = mmioRead(
            wave->fileHandle,
            (HPSTR)(&s_sample),
            numBytesToRead
        );

        if (numBytesRead != numBytesToRead) {
            return false;
        }

        (*f_sample) = shortToFloat(s_sample);

        return true;
    }

    void closeWithError(Wave* wave) {
        std::cout << "WaveReader error" << std::endl;
        close(wave);
    }

    void close(Wave* wave) {
        if (wave && wave->fileHandle) mmioClose(wave->fileHandle, 0);
        if (wave) HeapFree(GetProcessHeap(), 0, wave);
    }

private:
    // scale range (-((1 << 15) - 1), ((1 << 15) - 1)) to range (-1.0f, 1.0f)
    float shortToFloatRatio = 1.0f / ((1 << 15) - 1);

    // convert 16-bit signed int to float in range (-1.0f, 1.0f)
    float shortToFloat(SHORT s) {
        return ((float)s) * shortToFloatRatio;
    }
};
