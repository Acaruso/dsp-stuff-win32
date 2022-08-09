#pragma once

#include "../lib/readerwriterqueue.h"

#include "src/shared/shared_data.hpp"

int audioMain(SharedData* sharedData);
// int audioMain(moodycamel::ReaderWriterQueue<std::string>* queue);
