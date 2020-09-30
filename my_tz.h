#ifndef __MY_TZ_H__
#define __MY_TZ_H__

#include <AceTime.h>

#ifdef __TZMAIN__
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

#define TZ_AEST    0x0fe559a3	// Eastern Australia (Melbourne) +10
#define TZ_ACST    0x2428e8a3	// Central Australia (Adelaide) +9:30
#define TZ_AWST    0x8db8269d	// Western Australia (Perth) +8
#define TZ_CHINA   0x577f28ac	// People's Republic of China +8
#define TZ_ICT     0x20f2d127	// Indochina Time (Vietnam) +7
#define TZ_NPT     0x9a96ce6f	// Nepal +5:45
#define TZ_IST     0x72c06cd9	// India (Calcutta, New Delhi, etc.) +5:30
#define TZ_AST     0x9ceffbed	// Arabia Standard Time (Iraq, Iran) +3
#define TZ_EET     0x4318fa27	// Eastern European Time (Athens) +2
#define TZ_CET     0xa2c58fd7	// Central European Time(Rome, Madrid) +1
#define TZ_WET     0x5c6a84ae	// Western European Time (London)
#define TZ_GMT     0xd8e31abc	// Greenwich Mean Time
#define TZ_AZO     0xf93ed918	// Azores Standard Time -1
#define TZ_OSC     0x33013174	// Oscar Standard Time (and South Georgia) -2
#define TZ_WGT     0x9805b5a9	// Western Greenland Time (Nuuk) -3
#define TZ_ATL     0xbc5b7183	// Atlantic Standard Time (Bermuda, Labrador)
#define TZ_EST     0x1e2a7654	// Eastern Standard Time -5
#define TZ_CST     0x4b92b5d4	// Central Standard Time -6
#define TZ_MST     0x97d10b2a	// Mountain Standard Time -7
#define TZ_ARIZONA 0x34b5af01	// Arizona -7
#define TZ_PST     0xd99ee2dc	// Pacific Standard Time -8
#define TZ_AKST    0x5a79260e	// Alaska Standard Time -9
#define TZ_HST     0xe6e70af9	// Hawaii-Aleutian Standard Time -10

struct tz_opt {
    uint32_t tzone;
    char *tzname;
};

EXTERN struct tz_opt tz_opts[]
#ifdef __TZMAIN__
		      = {
			  { TZ_AEST, "Melbourne" },
			  { TZ_ACST, "Adelaide" },
			  { TZ_AWST, "Perth" },
			  { TZ_CHINA, "China" },
			  { TZ_ICT, "Vietnam" },
			  { TZ_NPT, "Nepal" },
			  { TZ_IST, "India" },
			  { TZ_AST, "Iraq" },
			  { TZ_EET, "Athens" },
			  { TZ_CET, "Rome" },
			  { TZ_WET, "London" },
			  { TZ_GMT, "GMT/UTC" },
			  { TZ_AZO, "Azores" },
			  { TZ_OSC, "Oscar" },
			  { TZ_WGT, "Greenland" },
			  { TZ_ATL, "Atlantic CAN" },
			  { TZ_EST, "New York" },
			  { TZ_CST, "Central" },
			  { TZ_MST, "Mountain" },
			  { TZ_ARIZONA, "Arizona" },
			  { TZ_PST, "Pacific LAX" },
			  { TZ_AKST, "Anchorage" },
			  { TZ_HST, "Hawaii" }
			}
#endif
;

EXTERN int sizeof_tz_opts INIT(sizeof(tz_opts)/sizeof(struct tz_opt));
EXTERN uint32_t tzindex;
EXTERN uint8_t ahh, amm;

void init_timezones(void);
void get_time_in_tz(uint32_t tzoneId);
void convert_alarm_time_to_gmt(int8_t *, int8_t *);

#endif
