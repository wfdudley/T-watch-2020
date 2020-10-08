// Dan Geiger's LCARS Time Display
// This is a drop-in replacement for the displayTime()
// In the original Instructable. As such, it redraws GUI
// every minute so you will see a little flicker.
//
// heavily modified by W.F.Dudley Jr.

#include <AceTime.h>
#include <time.h>
#include <soc/rtc.h>
#include "config.h"
#include "DudleyWatch.h"
#include "my_tz.h"

using namespace ace_time;

// this will display 80% when percentage is between 70% and 90%
// fully charged battery never exceeds 98% as far as I can tell.
#define BATT_ICON_WIDTH 48
#define BATT_ICON_HEIGHT 18
#define BHOVER3 (BATT_ICON_HEIGHT/3)
#define BWOVER5 ((BATT_ICON_WIDTH/5)-1)
#define BCHGT (BATT_ICON_HEIGHT - 6)
void battery_icon(uint16_t ox, uint16_t oy, uint16_t fgc, uint16_t bgc){
int per, pp10;
  do {
    per = ttgo->power->getBattPercentage();
  } while(per > 100);
  pp10 = per + 10 ;
  tft->fillRect(ox, oy, BATT_ICON_WIDTH, BATT_ICON_HEIGHT, bgc);
  tft->drawRect(ox, oy, BATT_ICON_WIDTH - 3, BATT_ICON_HEIGHT, fgc);
  tft->fillRect(ox + BATT_ICON_WIDTH - 3, oy + BHOVER3, 3, BHOVER3, fgc);
  for(int i = 0 ; i < 5 ; i++) {
    if(((i+1) * 20) < pp10) {
      tft->fillRect(4 + ox + (BWOVER5 * i), oy + 3, BWOVER5-2, BCHGT, fgc);
    }
  }
}

void charge_icon(uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor, boolean rounded){
uint16_t li = ox + 8;
uint16_t ri = ox + 16;
  if(rounded) {
    tft->fillRoundRect(ox, oy, 24, 20, 10, bgcolor);
  }
  else {
    tft->fillRect(ox, oy, 24, 20, bgcolor);
  }
  if (charge_cable_connected) {
    tft->drawLine(li, oy + 15, ri, oy +  9, fgcolor);
    tft->drawLine(li, oy + 14, ri, oy +  8, fgcolor);
    tft->drawLine(li, oy +  9, ri, oy +  9, fgcolor);
    tft->drawLine(li, oy +  8, ri, oy +  8, fgcolor);
    tft->drawLine(li, oy +  9, ri, oy +  4, fgcolor);
    tft->drawLine(li, oy +  8, ri, oy +  3, fgcolor);
  }
}

static void draw_bat_percent(uint16_t ox, uint16_t oy) {
int per;
  tft->setTextColor(TFT_GREEN);
  do {
    per = ttgo->power->getBattPercentage();
  } while(per > 100);
  tft->drawString(String(per) + "%", ox, oy, 2);
}

void steps_icon (uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor, uint8_t mult) {
  if(mult < 1) { mult = 1; }
  else if(mult > 4) { mult = 4; }
  // Serial.printf("ox = %d, oy = %d, mult = %d\n", ox, oy, mult);
  tft->fillRect(ox, oy, mult * 22, mult * 20, bgcolor);
  // fillRoundRect(x, y, w, h, radius, color)
  // left foot
  tft->fillCircle(ox + (6 * mult), oy + (14 * mult), 1 * mult, fgcolor);//heel
  tft->fillRoundRect(ox + (4 * mult), oy + (6 * mult), 4 * mult, 6 * mult, 2 * mult, fgcolor);
  // right foot
  tft->fillCircle(ox + (13 * mult), oy + (11 * mult), 1 * mult, fgcolor);//heel
  tft->fillRoundRect(ox + (11 * mult), oy + (3 * mult), 4 * mult, 6 * mult, 2 * mult, fgcolor);
}

void draw_step_counter_rjust (uint16_t ox, uint16_t oy, uint16_t bgcolor, uint16_t fgcolor, uint8_t font) {
    if(general_config.stepcounter_filter
    && ((step_counter != last_step_counter) || !steps_is_up)) {
      // refresh step counter
      ttgo->tft->setTextColor(fgcolor, bgcolor);
      tft->drawRightString(String(step_counter), ox, oy + 2, font);
      steps_icon(ox, oy, TFT_WHITE, TFT_BLACK, 1);
      last_step_counter = step_counter;
      steps_is_up = true;
    }
}

// fullUpdate == 2 -> redraw everything
// fullUpdate == 1 -> redraw hh:mm and battery %
// fullUpdate == 0 -> update seconds indicator
void LCARS_Time(uint8_t fullUpdate) {
static uint8_t last_dday;
  tft->setTextDatum(TL_DATUM);
  // Serial.println(F("LCARS_Time"));
  ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);

  get_time_in_tz(tzindex);
  // these is used for the alarm function:
  local_hour = hh;
  local_minute = mm;

  tft->setTextSize(1);

  if (fullUpdate == 2 || dday != last_dday) {
    //Draw the back graphics - Top of display
    tft->fillScreen(TFT_BLACK);
    // fillRoundRect(x, y, w, h, radius, color)
    tft->fillRoundRect(  0, 0, 239, 120, 40, TFT_PURPLE);
    tft->fillRoundRect(200, 0,  39,  20, 10, TFT_PURPLE); // rounded end, upr rt
    tft->fillRoundRect(40, 20, 196, 80, 20, TFT_BLACK); // big upper cut-out
    tft->fillRect(80, 20, 159, 80, TFT_BLACK); // right side of big cut-out
    // tft->fillRect(170, 0, 45, 20, TFT_BLACK); // behind battery: 98%
    tft->fillRect(157, 0, 4, 20, TFT_BLACK); // cutout before battery_icon
    tft->fillRect(0, 45, 40, 7, TFT_BLACK); // upper small horiz cut-out
    tft->fillRect(0, 70, 40, 7, TFT_BLACK); // lower small horiz cut-out

    //Draw the back graphics - Bottom of display
    tft->fillRoundRect(0, 130, 239, 109, 40, TFT_MAROON); // big oval
    tft->fillRoundRect(40, 151, 199, 80, 20, TFT_BLACK); // big cut-out
    tft->fillRoundRect(200, 130, 39, 21, 9, TFT_MAROON); // rounded end, upr rt
    tft->fillRoundRect(200, 231, 39,  8, 4, TFT_MAROON); // rounded end, low rt
    tft->fillRect(80, 151, 159, 80, TFT_BLACK); // right side of big cut-out
    tft->fillRect(0, 179, 50, 10, TFT_BLACK); // small horiz cut-out
    tft->fillRoundRect(100, 160, 50, 10, 5, TFT_YELLOW); // low temp
    tft->fillRoundRect(170, 160, 50, 10, 5, TFT_RED); // high temp
    tft->fillRect(140, 160, 40, 10, TFT_DARKGREEN); // medium temp
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString("Temp", 66, 158, 2);
    tft->fillRoundRect(119, 199, 120, 27, 15, TFT_DARKCYAN);

    // Display Temp Marker - you may need to adjust the x value based on your normal ADC results
    float temp = ttgo->power->getTemp();
    tft->fillRoundRect(int(temp) - 20, 170, 10, 20, 5, TFT_WHITE);

    // Display Time
    // Font 7 is a 7-seg display but only contains
    // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .

    tft->setTextColor(0xFBE0, TFT_BLACK);
    int xpos = 55;
    if (hh < 10) xpos += tft->drawChar('0', xpos, 35, 7);
    xpos += 3 + tft->drawNumber(hh, xpos, 35, 7);
    xpos += tft->drawChar(':', xpos, 35, 7);
    if (mm < 10) xpos += tft->drawChar('0', xpos, 35, 7);
    tft->drawNumber(mm, xpos, 35, 7);
    if(if_not_home_tz()) {
      tft->setTextColor(TFT_BLACK, TFT_MAROON);
      sprintf(buff, "home %02d:%02d", home_hh, home_mm);
      tft->drawCentreString(buff, half_width, 130, 4);
      tft->fillRect(53, 151, 162, 5, TFT_BLACK);
    }

    battery_icon(161, 1, TFT_RED, TFT_BLACK);

#if PERCENT_AS_NUMBER
    // battery level value:
    tft->setTextColor(TFT_GREEN);
    draw_bat_percent(179, 2);
#endif
    // day of the month:
    tft->setTextColor(TFT_GREENYELLOW);
    tft->drawString(String(dday), 50, 187, 6);

    // Turn off the battery adc
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, false);

    // write month
    tft->setTextColor(TFT_WHITE);
    tft->drawString(DateStrings().monthShortString(mmonth), 9, 194, 2);
  }
  else if(fullUpdate == 1) {
    tft->fillRoundRect(40, 20, 196, 80, 20, TFT_BLACK); // big upper cut-out
    tft->fillRect(80, 20, 159, 80, TFT_BLACK); // right side big cut-out
    // tft->fillRect(170, 0, 45, 20, TFT_BLACK); // behind battery: 98%

    // hh:mm
    tft->setTextColor(0xFBE0, TFT_BLACK);
    int xpos = 55;
    if (hh < 10) xpos += tft->drawChar('0', xpos, 35, 7);
    xpos += 3 + tft->drawNumber(hh, xpos, 35, 7);
    xpos += tft->drawChar(':', xpos, 35, 7);
    if (mm < 10) xpos += tft->drawChar('0', xpos, 35, 7);
    tft->drawNumber(mm, xpos, 35, 7);
    if(if_not_home_tz()) {
      tft->setTextColor(TFT_BLACK, TFT_MAROON);
      sprintf(buff, "home %02d:%02d", home_hh, home_mm);
      tft->drawCentreString(buff, half_width, 130, 4);
      tft->fillRect(53, 151, 162, 5, TFT_BLACK);
    }

#if PERCENT_AS_NUMBER
    // battery level value:
    draw_bat_percent(179, 2);
#endif
  }

  draw_step_counter_rjust(217, 100, TFT_BLACK, TFT_GREEN, 2);
  // draw_step_counter_rjust( ox,  oy, bgcolor, fgcolor, font);
  charge_icon(215, 0, TFT_WHITE, TFT_PURPLE, true);
  tft->fillRect(211, 0, 4, 20, TFT_BLACK); // cutout before charge_icon
  // Build a bargraph every 10 seconds
  int secmod = ss % 10;
  if (secmod) { // Show growing bar every 10 seconds
    tft->fillRect(126 + secmod * 10, 205, 6, 15, TFT_ORANGE);
    // was 126 + secmod * 10, 215, 6, 15
  } else {
    tft->fillRoundRect(119, 199, 120, 27, 15, TFT_DARKCYAN);
    // tft->fillRoundRect(119, 197, 120, 29, 15, TFT_DARKCYAN);
    // was 119, 210, 120, 29, 15
  }
  last_dday = dday;
}
