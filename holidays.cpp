// identify holidays
// William F. Dudley Jr. 2020 11 09

#include "config.h"
#include "DudleyWatch.h"
#include <stdio.h>

#define __HOLIMAIN__ 1
#include "holidays.h"

char * is_holiday(int month, int day, int dow) {
    for(int i = 0 ; i < sizeof_Holidays ; i++) {
	if(month == Holidays[i].month) {
	    if(Holidays[i].fixed_day
	    && day == Holidays[i].fixed_day) {
		return Holidays[i].name;
	    }
	    else if(Holidays[i].min_day
		 && day >= Holidays[i].min_day
		 && day <= Holidays[i].max_day
		 && dow == Holidays[i].day_of_week) {
		return Holidays[i].name;
	    }
	}
    }
    return NULL;
}

#if OLD_WAY
char * is_holiday(int month, int day, int dow) {
    if(month == 1 && day == 1) return "New Years Day";
    if(month == 2 && day >= 15 && day <= 21 && dow == 1) return "President's Day";
    if(month == 7 && day == 4) return "Independence Day";
    if(month == 5 && day >=24 && day <= 30 and dow == 1) return "Memorial Day";
    if(month == 9 && day <= 7 && dow == 1) return "Labor Day";
    if(month == 11 && day >= 22 && day <= 28 && dow == 4) return "Thanksgiving";
    if(month == 12 && day == 25) return "Christmas";
    return NULL;
}
#endif
