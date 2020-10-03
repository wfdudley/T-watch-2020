// Dan Geiger's time setting routine
// Set the time - no error checking, you might want to add it
//
// heavily modified by W.F.Dudley Jr.
//    with new keyboard display.

#include "config.h"
#include <soc/rtc.h>
#include "DudleyWatch.h"

void prtTime(byte);

const char *number_labels[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "CANCEL", "0", "DONE" };

void flash_keyboard_item (const char **b_labels, uint8_t font, bool leave_room_for_label, int row, int col) {
uint8_t yvals[4], yh;
uint16_t icolor;
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t xvals[3] = { 0, 83, 164 };
  int16_t xtvals[3] = { 41, 120, 200 };
  // note: space at the top do display what is typed
  // was ttgo->tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  ttgo->tft->setTextColor(TFT_GREEN);
  int ino = col + (row * 3);
  icolor = TFT_LIGHTGREY;
  ttgo->tft->fillRoundRect(xvals[col], yvals[row], 75, yh-5, 6, icolor);
  ttgo->tft->drawCentreString(b_labels[ino], xtvals[col], yvals[row]+5, font);
  delay(100);
  icolor = (!strcmp(b_labels[ino], "CANCEL") || !strcmp(b_labels[ino], "DONE")) ? TFT_DARKGREY : TFT_BLUE ;
  ttgo->tft->fillRoundRect(xvals[col], yvals[row], 75, yh-5, 6, icolor);
  ttgo->tft->drawCentreString(b_labels[ino], xtvals[col], yvals[row]+5, font);
}

void draw_keyboard (uint8_t num_keys, const char **b_labels, uint8_t font, bool leave_room_for_label, char *top_label) {
uint8_t yvals[4], yh, row, col;
uint16_t icolor;
  ttgo->tft->fillScreen(TFT_BLACK);
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
    if(top_label) {
      ttgo->tft->setTextColor(TFT_GREEN);
      ttgo->tft->drawCentreString( top_label, half_width, 5, font);
    }
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t xvals[4];
  int16_t xtvals[4];
  xvals[0] = 0;
  if(num_keys == 12) {
    for(int i = 0 ; i < 3 ; i++) {
      xvals[i]  = i * (246 / 3);
      xtvals[i] = xvals[i] + (246 / 6);
    }
  }
  else {
    for(int i = 0 ; i < 4 ; i++) {
      xvals[i]  = i * (248 / 4);
      xtvals[i] = xvals[i] + (248 / 8);
    }
  }
  // note: space at the top do display what is typed
  // was ttgo->tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  ttgo->tft->setTextColor(TFT_GREEN);
  for(row = 0 ; row < 4 ; row++) {
    for(col = 0 ; col < (num_keys / 4) ; col++) {
      int ino = col + (row * (num_keys / 4));
      icolor = (!strcmp(b_labels[ino], "CANCEL") || !strcmp(b_labels[ino], "DONE")) ? TFT_DARKGREY : TFT_BLUE ;
      ttgo->tft->fillRoundRect(xvals[col], yvals[row],
	(num_keys == 12) ? 75 : 55, yh-5, 6, icolor);
      ttgo->tft->drawCentreString(b_labels[ino], xtvals[col], yvals[row]+5, font);
    }
  }
}

void appSetTime(void) {
  // Get the current info
  RTC_Date tnow = ttgo->rtc->getDateTime();

  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;

  //Draw the numerical keypad:

  draw_keyboard(12, number_labels, 2, true, NULL);

  int wl = 0; // Track the current number selected
  byte curnum = 1;  // Track which digit we are on

  prtTime(curnum); // Display the time for the current digit

  while (wl != 13 && wl != 0x1b) {
    wl = get_numerical_button_press();
    if (wl != -1 && wl != 13 && wl != 0x1b) {
    int row, col;
    row = (wl > 0) ? (wl - 1) / 3 : 3 ;
    col = (wl > 0) ? (wl - 1) % 3 : 1 ;
    flash_keyboard_item(number_labels, 2, true, row, col);

      switch (curnum) {
        case 1:
	  // Serial.printf("(1) wl = %d, hh = %d; ", wl, hh);
          hh = wl * 10 + hh % 10;
	  // Serial.printf("NOW hh = %d\n", hh);
          break;
        case 2:
	  // Serial.printf("(2) wl = %d, hh = %d; ", wl, hh);
          hh = int(hh / 10) * 10 + wl;
	  // Serial.printf("NOW hh = %d\n", hh);
          break;
        case 3:
          mm = wl * 10 + mm % 10;
          break;
        case 4:
          mm = int(mm / 10) * 10 + wl;
          break;
        case 5:
          dday = wl * 10 + dday % 10;
          break;
        case 6:
          dday = int(dday / 10) * 10 + wl;
          break;
        case 7:
          mmonth = wl * 10 + mmonth % 10;
          break;
        case 8:
          mmonth = int(mmonth / 10) * 10 + wl;
          break;
      }
      while (get_numerical_button_press() != -1) {
	my_idle();
      }
      curnum++;
      if (curnum > 8) curnum = 1;
      prtTime(curnum);
    }
  }
  if(wl == 0x1b) {
    flash_keyboard_item(number_labels, 2, true, 3, 0);
    return;
  }
  flash_keyboard_item(number_labels, 2, true, 3, 2);
  while (get_numerical_button_press() != -1) {
    my_idle();
  }
  tnow = ttgo->rtc->getDateTime();
  ss = tnow.second;
  // Serial.printf("line %d hh = %d\n", __LINE__, hh);
  Serial.printf("appsetTime: setting time to hh:mm:ss %d:%02d:%02d\n", hh, mm, ss);
  ttgo->rtc->setDateTime(yyear, mmonth, dday, hh, mm, ss);
  ttgo->tft->fillScreen(TFT_BLACK);
}

// prtTime will display the current selected time and highlight
// the current digit to be updated in yellow

void prtTime(byte digit) {
  ttgo->tft->fillRect(0, 0, 200, 34, TFT_BLACK);
  if (digit == 1)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(int(hh / 10), 5, 5, 2);
  if (digit == 2)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(hh % 10, 25, 5, 2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawString(":",  45, 5, 2);
  if (digit == 3)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(int(mm / 10), 65 , 5, 2);
  if (digit == 4)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(mm % 10, 85, 5, 2);

  if (digit == 5)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(int(dday / 10), 115, 5, 2);
  if (digit == 6)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(dday % 10, 135, 5, 2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawString("/",  155, 5, 2);
  if (digit == 7)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(int(mmonth / 10), 175 , 5, 2);
  if (digit == 8)   ttgo->tft->setTextColor(TFT_YELLOW);
  else   ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawNumber(mmonth % 10, 195, 5, 2);
}

// get_numerical_button_press takes care of translating where we pressed into
// a number that was pressed. Returns -1 for no press
// and 13 for DONE, 0x1b for CANCEL

int get_numerical_button_press(void) {
  int16_t x, y;
  if (!ttgo->getTouch(x, y)) return - 1;
  if (y < 85) {
    if (x < 80) return 1;
    else if (x > 160) return 3;
    else return 2;
  }
  else if (y < 135) {
    if (x < 80) return 4;
    else if (x > 160) return 6;
    else return 5;
  }
  else if (y < 185) {
    if (x < 80) return 7;
    else if (x > 160) return 9;
    else return 8;
  }
  else if (x < 80) return 0x1b;
  else if (x > 160) return 13;
  else return 0;
}
