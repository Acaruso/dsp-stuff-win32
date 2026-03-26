#include "audio_main.hpp"

#include <comdef.h>
#include <string>
#include <xmmintrin.h>

#include "src/audio/audio_service.hpp"
#include "src/audio/wasapi_client.hpp"

int audioMain(SharedData* sharedData) {
    // set flush-to-zero and denormals-are-zero mode (SSE2)
    // see Agner Fog optimization manual example 7.5
    // https://www.agner.org/optimize/optimizing_cpp.pdf
    _mm_setcsr(_mm_getcsr() | 0x8040);

    // initialize COM:
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    try {
        WasapiClient wasapiClient;
        AudioService audioService{wasapiClient, sharedData};
        audioService.run();
    } catch(std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
