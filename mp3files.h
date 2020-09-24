// mp3's of beeps of various frequencies
// W.F.Dudley Jr.

#include "beep__182.h"	// 182 Hz 100 msec
#include "beep__220.h"	// 220 Hz 100 msec
#include "beep__279.h"	// etc.
#include "beep__303.h"
#include "beep__473.h"
#include "beep__576.h"
#include "beep_1050.h"	// 1050 Hz 100 msec
#include "beep_1050L.h"	// 1050 Hz 200 msec
#include "beep-chirp1.h"

const unsigned char* const beep_array[] PROGMEM = {
    beep__182_mp3,
    beep__220_mp3,
    beep__279_mp3,
    beep__303_mp3,
    beep__473_mp3,
    beep__576_mp3,
    beep_1050_mp3,
    beep_1050L_mp3,
    beep_chirp1_mp3
};

const uint16_t beep_sizes[] = {
    sizeof(beep__182_mp3),
    sizeof(beep__220_mp3),
    sizeof(beep__279_mp3),
    sizeof(beep__303_mp3),
    sizeof(beep__473_mp3),
    sizeof(beep__576_mp3),
    sizeof(beep_1050_mp3),
    sizeof(beep_1050L_mp3),
    sizeof(beep_chirp1_mp3)
};
