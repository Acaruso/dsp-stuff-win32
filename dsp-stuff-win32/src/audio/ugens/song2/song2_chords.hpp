#pragma once

#include "src/audio/ugens/song2/song2_freqs_notes.hpp"
#include "src/audio/ugens/song2/song2_note_util.hpp"

namespace Song2 {

inline Notes eMaj = Notes(
    guitar(5, 0),
    guitar(4, 0),
    guitar(3, 1),
    guitar(2, 2),
    guitar(1, 2),
    guitar(0, 0)
);

inline Notes eMajPlus2 = Notes(
    guitar(5, 2),
    guitar(4, 0),
    guitar(3, 1),
    guitar(2, 2),
    guitar(1, 2),
    guitar(0, 0)
);

inline Notes eMajPlus4 = Notes(
    guitar(5, 4),
    guitar(4, 0),
    guitar(3, 1),
    guitar(2, 2),
    guitar(1, 2),
    guitar(0, 0)
);

inline Notes aMin = Notes(
    guitar(5, 0),
    guitar(4, 1),
    guitar(3, 2),
    guitar(2, 2),
    guitar(1, 0)
);

inline Notes aMinPlus = Notes(
    guitar(5, 3),
    guitar(4, 1),
    guitar(3, 2),
    guitar(2, 2),
    guitar(1, 0)
);

inline Notes dMaj = Notes(
    guitar(5, 2),
    guitar(4, 3),
    guitar(3, 2),
    guitar(2, 0)
);

inline Notes dMajUp = Notes(
    guitar(5, 2 + 3),
    guitar(4, 3 + 3),
    guitar(3, 2 + 3),
    guitar(2, 0)
);

inline Notes cMaj = Notes(
    guitar(5, 0),
    guitar(4, 0),
    guitar(3, 0),
    guitar(2, 2),
    guitar(1, 3)
);

inline Notes cMajPlus = Notes(
    guitar(5, 0),
    guitar(4, 3),
    guitar(3, 0),
    guitar(2, 2),
    guitar(1, 3)
);

inline Notes cMajUp = Notes(
    guitar(5, 0),
    guitar(4, 3 + 2),
    guitar(3, 0),
    guitar(2, 2 + 2),
    guitar(1, 3 + 2)
);

}
