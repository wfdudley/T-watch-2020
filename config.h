// This is the basic config file that defines which
// LILYGO product you are compiling for

#define LILYGO_WATCH_2020_V1              // Use T-Watch2020
#define LILYGO_WATCH_LVGL 

#include <LilyGoWatch.h>

#define EEPROM_SIZE 512

// Except T-Watch2020, other versions need to be selected according to the actual situation
#ifndef LILYGO_WATCH_2020_V1

// T-Watch comes with the default backplane, it uses internal DAC
#define STANDARD_BACKPLANE

// Such as MAX98357A, PCM5102 external DAC backplane
// #define EXTERNAL_DAC_BACKPLANE

#else
// T-Watch2020 uses external DAC
#undef STANDARD_BACKPLANE
#define EXTERNAL_DAC_BACKPLANE

#endif


#include <LilyGoWatch.h>
