// my_tz.h
//
#include "Timezone.h"

#ifdef __TZMAIN__
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

typedef struct tz_opt {
    int tzone;
    char *tzname;
} TZ_OPT;

EXTERN int8_t tzindex;

EXTERN char * tz_select_start INIT("\t<td>\n\t<select name=\"tz%02d\">\n\t<option value=\"\">Choose timezone</option>\n");
EXTERN char * tz_select_end   INIT("\t</select>\n");
EXTERN char * tz_select_option INIT("\t<option value=\"%d\" %s>%s</option>\n");

// IF YOU ADD ANOTHER tz_opt, remember to edit timelocal(), in my_tz.cpp.

EXTERN TZ_OPT tz_opts[]
#ifdef __TZMAIN__
	  = {
		{ 2, "Eastern Eur" },
		{ 1, "Central Eur" },
		{ 0, "GMT" },
		{ -1, "Azores" },
		{ -2, "Oscar" },
		{ -3, "Greenland" },
		{ -4, "Atlantic CAN" },
		{ -5, "Eastern NYC" },
		{ -6, "Central CHI" },
		{ -7, "Mountain" },
		{ 27, "Arizona" },
		{ -8, "Pacific LAX" },
		{ -9, "Alaska" },
		{ -10, "W. Alaska" },
		{ 28, "Hawaii" },
#if 0
		{ -11, "Midway Island, Samoa" },
#endif
		{ 10, "E Australia (Melbourne, Sydney)" },
		{ 7, "China" }
	    }
#endif
;

EXTERN int sizeof_tz_opts INIT(sizeof(tz_opts)/sizeof(TZ_OPT));

#ifndef __MY_TZ_H__
#define __MY_TZ_H__

#define TZ_ARIZONA 27	// Arizona
#define TZ_HAWAII 28	// Hawaii
#define TZ_AEST 10	// Eastern Australia
#define TZ_CHINA 7	// People's Republic of China
#define TZ_EET  2	// Eastern European Time
#define TZ_CEST 2	// Central European Savings Time
#define TZ_CET  1	// Central European Time
#define TZ_GMT  0	// Greenwich Mean Time
#define TZ_AZO -1	// Azores Standard Time
#define TZ_OSC -2	// Oscar Standard Time (and a Brazilian Island)
#define TZ_WGT -3	// Western Greenland Time ("standard")
#define TZ_WGST -2	// Western Greenland Summer Time ("daylight")
#define TZ_ATL -4	// Atlantic Standard Time
#define TZ_EST -5	// Eastern Standard Time
#define TZ_EDT -4	// Eastern Daylight Time
#define TZ_CST -6	// Central Standard Time
#define TZ_CDT -5	// Central Daylight Time
#define TZ_MST -7	// Mountain Standard Time
#define TZ_MDT -6	// Mountain Daylight Time
#define TZ_PST -8	// Pacific Standard Time
#define TZ_PDT -7	// Pacific Daylight Time
#define TZ_AKST -9	// Alaska Standard Time
#define TZ_HST -10	// Hawaii-Aleutian Standard Time

time_t timelocal(time_t);

#endif
