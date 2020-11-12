// identify holidays
// William F. Dudley Jr. 2020 11 09
// WFD added birthdays/anniversarys etc.  2020 11 11

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

char * is_birthday(int month, int day, int dow) {
    for(int i = 0 ; i < sizeof_Birthdays ; i++) {
	if(month == Birthdays[i].month) {
	    if(Birthdays[i].fixed_day
	    && day == Birthdays[i].fixed_day) {
		return Birthdays[i].name;
	    }
	    else if(Birthdays[i].min_day
		 && day >= Birthdays[i].min_day
		 && day <= Birthdays[i].max_day
		 && dow == Birthdays[i].day_of_week) {
		return Birthdays[i].name;
	    }
	}
    }
    return NULL;
}

