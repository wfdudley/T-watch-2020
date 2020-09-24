#include "my_tz.h"

// "abbrev", week, day, month, hour change occurs, offset in minutes
// week is an enum: Last, First, Second, Third, Fourth
// day is an enum: Sun, Mon, . . . Sat
// month is an enum: Jan, Feb . . . Dec

// IF YOU ADD ANOTHER ZONE, remember to edit timelocal(), below.

TimeChangeRule EEST = {"EEST", Last, Sun, Mar, 0, 180};   //Eastern European Time (Athens, Bucharest, Beirut)
TimeChangeRule EET = {"EET ", Last, Sun, Oct, 0, 120};    //Eastern European Time (Athens, Bucharest, Beirut)
Timezone EE(EEST, EET);

//TimeZone Settings Details https://github.com/JChristensen/Timezone
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};   //Central European Time (Frankfurt, Paris)
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};     //Central European Time (Frankfurt, Paris)
Timezone CE(CEST, CET);

// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};      // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};       // Standard Time
Timezone UK(BST, GMT);

TimeChangeRule AZOST = {"AZOST", Last, Sun, Mar, 0, 0};   // Azores Savings Time (Azores)
TimeChangeRule AZOT = {"AZOT ", Last, Sun, Oct, 1, -60};  // Azores Time (Azores)
Timezone AZO(AZOST, AZOT);

// "Oscar", mid Atlantic, and Fernando de Noronha, Pernambuco, Brazil
TimeChangeRule OSC = {"OSC", First, Sat, Oct, 23, -120};   //UTC - 2 hours
Timezone Oscar(OSC, OSC);

// "Western" (most of) Greenland
TimeChangeRule WGST = {"WGST", Fourth, Sat, Mar, 22, -120};  //UTC - 2 hours
TimeChangeRule WGT = {"WGT", Third, Sat, Oct, 23, -180};   //UTC - 3 hours
Timezone GreenlandNuuk(WGST, WGT);

TimeChangeRule ADT = {"ADT", Second, Sun, Mar, 2, -180};  //UTC - 3 hours
TimeChangeRule AST = {"AST", First, Sun, Nov, 2, -240};   //UTC - 4 hours
Timezone Atlantic(ADT, AST);

TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
Timezone usEastern(usEDT, usEST);

//US Central Time Zone (Chicago, Houston)
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
Timezone usCentral(usCDT, usCST);

//US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
Timezone usMountain(usMDT, usMST);

//Arizona is US Mountain Time Zone but does not use DST
Timezone usArizona(usMST, usMST);

//US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPacific(usPDT, usPST);

// Eastern Alaska
TimeChangeRule usAKDT = {"AKDT", First, Sun, Mar, 2, -480};
TimeChangeRule usAKST = {"AKST", First, Sun, Nov, 2, -540};
Timezone usAkEast(usAKDT, usAKST);

// Western Alaska and the Aleutians
TimeChangeRule usHDT = {"HDT", First, Sun, Mar, 2, -540};
TimeChangeRule usHST = {"HST", First, Sun, Nov, 2, -600};
Timezone usAkWest(usHDT, usHST);

// Hawaii, doesn't use DST
Timezone usHawaii(usHST, usHST);

// Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    // UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    // UTC + 10 hours
Timezone ausET(aEDT, aEST);

TimeChangeRule CHT = {"China", Last, Sun, Mar, 2, 480};     // China
Timezone ChinaRule(CHT, CHT);

//Pointer To The Time Change Rule, Use to Get The TZ Abbrev
TimeChangeRule *tcr;

// looks at the global tzone, which is my arbitrary index into
// my own list of time zones.
time_t timelocal(time_t utctime) {
time_t rt;
    if(tzindex == TZ_EET) {
	rt = EE.toLocal(utctime);
    }
    else if(tzindex == TZ_CET) {
	rt = CE.toLocal(utctime);
    }
    else if(tzindex == TZ_GMT) {
	rt = UK.toLocal(utctime);
    }
    else if(tzindex == TZ_AZO) {
	rt = AZO.toLocal(utctime);
    }
    else if(tzindex == TZ_OSC) {
	rt = Oscar.toLocal(utctime);
    }
    else if(tzindex == TZ_WGT) {
	rt = GreenlandNuuk.toLocal(utctime);
    }
    else if(tzindex == TZ_ATL) {
	rt = Atlantic.toLocal(utctime);
    }
    else if(tzindex == TZ_EST) {
	rt = usEastern.toLocal(utctime);
    }
    else if(tzindex == TZ_CST) {
	rt = usCentral.toLocal(utctime);
    }
    else if(tzindex == TZ_MST) {
	rt = usMountain.toLocal(utctime);
    }
    else if(tzindex == TZ_ARIZONA) {
	rt = usArizona.toLocal(utctime);
    }
    else if(tzindex == TZ_PST) {
	rt = usPacific.toLocal(utctime);
    }
    else if(tzindex == TZ_HAWAII) {
	rt = usHawaii.toLocal(utctime);
    }
    else if(tzindex == TZ_AKST) {
	rt = usAkEast.toLocal(utctime);
    }
    else if(tzindex == TZ_HST) {
	rt = usAkWest.toLocal(utctime);
    }
    else if(tzindex == TZ_AEST) {
	rt = ausET.toLocal(utctime);
    }
    else if(tzindex == TZ_CHINA) {
	rt = ChinaRule.toLocal(utctime);
    }
    else {	// default, return GMT
	rt = UK.toLocal(utctime);
    }
    if(year(rt) < 2036) {
	return(rt);
    }
    else {
	return 0;
    }
}
