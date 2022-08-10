#pragma once

#include <cstdlib>

inline double getRand() {
    return rand() / (RAND_MAX + 1.0);
}
