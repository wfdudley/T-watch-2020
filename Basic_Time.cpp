// Dan Geiger's Basic Digital watch
// The basic Time Display GUI
// if you are just updating the colon, fullUpdate =0
// if you want to update the complete display, fullUpdate =1
// This helps reduce flicker
//
// modified by W.F. Dudley Jr.

#include <time.h>
#include <soc/rtc.h>
#include "config.h"
#include "DudleyWatch.h"
#include "my_tz.h"

byte xcolon = 0; // location of the colon

boolean if_not_home_tz(void) {
struct tm timeinfo;
time_t utc_time, unix_time;
  if(general_config.home_tzindex != tzindex) {
    timeinfo.tm_hour = tnow.hour;
    timeinfo.tm_min  = tnow.minute;
    timeinfo.tm_sec  = tnow.second;
    timeinfo.tm_mday = tnow.day;
    timeinfo.tm_mon  = tnow.month - 1;
    timeinfo.tm_year = tnow.year - 1900;
    utc_time = mktime(&timeinfo);
    tzindex = general_config.home_tzindex;
    unix_time = timelocal(utc_time);
    tzindex = general_config.local_tzindex;
    memcpy(&timeinfo, localtime(&unix_time), sizeof(struct tm)/sizeof(char));
    home_hh = timeinfo.tm_hour;
    home_mm = timeinfo.tm_min;
    home_ss = timeinfo.tm_sec;
    return true;
  }
  return false;
}

void Basic_Time(uint8_t fullUpdate) {

  byte xpos = 40; // Starting position for the display
  byte ypos = 90;

  // Get the current data
  tnow = ttgo->rtc->getDateTime();
#define TIME_IS_GMT 1
#if TIME_IS_GMT
struct tm timeinfo;
time_t utc_time, unix_time;
  timeinfo.tm_hour = tnow.hour;
  timeinfo.tm_min  = tnow.minute;
  timeinfo.tm_sec  = tnow.second;
  timeinfo.tm_mday = tnow.day;
  timeinfo.tm_mon  = tnow.month - 1;
  timeinfo.tm_year = tnow.year - 1900;
#if 0
  if(!(tnow.second % 5)) {
    Serial.print(F("UTC time: "));
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
#endif
  utc_time = mktime(&timeinfo);
  unix_time = timelocal(utc_time);
  memcpy(&timeinfo, localtime(&unix_time), sizeof(struct tm)/sizeof(char));
#if 0
  if(!(tnow.second % 5)) {
    Serial.printf("tzindex = %d, local time: ", tzindex);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
#endif
  hh = timeinfo.tm_hour;
  mm = timeinfo.tm_min;
  ss = timeinfo.tm_sec;
  // wday = timeinfo.tm_wday;
  dday = timeinfo.tm_mday;
  mmonth = 1 + timeinfo.tm_mon;
  yyear = 1900 + timeinfo.tm_year;
#else
  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;
#endif
  local_hour = hh;
  local_minute = mm;
  tft->setTextSize(1);

  if (fullUpdate) {
    // Font 7 is a 7-seg display but only contains
    // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .

    tft->setTextColor(0x39C4, TFT_BLACK); // Set desired color
    // tft->setTextSize(2); // WAY TOO BIG
    tft->drawString("88:88", xpos, ypos, 7);
    tft->setTextColor(0xFBE0, TFT_BLACK); // Orange
#define NEEDED_FOR_FLASHING_COLON 1
#if NEEDED_FOR_FLASHING_COLON
    if (hh < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    xpos += tft->drawNumber(hh, xpos, ypos, 7);
    xcolon = xpos + 3;
    xpos += tft->drawChar(':', xcolon, ypos, 7);
    if (mm < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    tft->drawNumber(mm, xpos, ypos, 7);
#else
    sprintf(buff, "%02d:%02d", hh, mm);
    tft->drawString(buff, xpos, ypos, 7);
#endif
    if(if_not_home_tz()) {
      sprintf(buff, "home: %02d:%02d", home_hh, home_mm);
      tft->drawCentreString(buff, half_width, 160, 4);
    }
  }

  if (ss % 2) { // Toggle the colon every second
    tft->setTextColor(0x39C4, TFT_BLACK);
    xpos += tft->drawChar(':', xcolon, ypos, 7);
    tft->setTextColor(0xFBE0, TFT_BLACK);
  } else {
    tft->drawChar(':', xcolon, ypos, 7);
  }
  draw_step_counter_ljust(0, 5, TFT_GREEN, TFT_BLACK, 3);
  charge_icon(215, 0, TFT_RED, TFT_BLACK, false);
  if (fullUpdate) {
    tft->setTextSize(1);
    tft->setTextFont(4);
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    // show battery percentage
    int per;
    do {
      per = ttgo->power->getBattPercentage();
    } while(per > 100);
    ttgo->tft->setCursor(165, 0);
    ttgo->tft->print(per);
    ttgo->tft->print("%");
    battery_icon(110, 1, TFT_RED, TFT_BLACK);


    // show the date
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->setCursor( 10, 210);
    tft->printf("%02d/%02d/%4d", mmonth, dday, yyear);
#if UGLY
    ttgo->tft->print(mmonth);
    ttgo->tft->print("/");
    ttgo->tft->print(dday);
    ttgo->tft->print("/");
    ttgo->tft->print(yyear);
#endif
    tft->setTextColor(0xFBE0, TFT_BLACK); // Orange
  }
}
