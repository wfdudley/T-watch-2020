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
  if(general_config.home_tzindex != tzindex) {
    get_time_in_tz(general_config.home_tzindex);
    home_hh = hh;
    home_mm = mm;
    home_ss = ss;
    get_time_in_tz(tzindex);
    return true;
  }
  return false;
}

void Basic_Time(uint8_t fullUpdate) {
int h12;
  byte xpos = 40; // Starting position for the display
  byte ypos = 90;

  // Get the current data
  get_time_in_tz(tzindex);
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
    h12 = cvt_12_hour_clock(hh);
#define NEEDED_FOR_FLASHING_COLON 1
#if NEEDED_FOR_FLASHING_COLON
    if (h12 < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    xpos += tft->drawNumber(h12, xpos, ypos, 7);
    xcolon = xpos + 3;
    xpos += tft->drawChar(':', xcolon, ypos, 7);
    if (mm < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    xpos += tft->drawNumber(mm, xpos, ypos, 7);
#else
    sprintf(buff, "%02d:%02d", h12, mm);
    xpos += tft->drawString(buff, xpos, ypos, 7);
#endif
    tft->drawString((!general_config.twelve_hr_clock) ? "  " : (hh < 12) ? "AM" : "PM", xpos + 3, ypos + 10, 4);
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
