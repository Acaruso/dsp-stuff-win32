#include "audio_main.hpp"

#include <comdef.h>
#include <string>

#include "src/audio/audio_service.hpp"
#include "src/audio/wasapi_client.hpp"

int audioMain(SharedData* sharedData) {
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
