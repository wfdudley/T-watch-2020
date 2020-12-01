// simple Calendar program, similar to "cal" on unix.
// William F. Dudley Jr. 2020 11 08
// part of "DudleyWatch"

// tap anywhere not on the buttons to exit.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "config.h"
#include "DudleyWatch.h"
#include "my_tz.h"

char * is_holiday(int month, int day, int dow);
char * is_birthday(int month, int day, int dow);

#define NUMBUTTONS 5

#define DBG 0
#if DBG
// this is so I can time how long the code takes.  mktime takes
// several seconds to run if the tm_mday value is silly.
long testtimes[20];
int testlines[20];
int ttp = 0;	// testtimes pointer -> index into testtimes[] and testlines[]

// save the elapsed time in msec and the (presumably line) number.
void savetime(int l) {
  testtimes[ttp] = millis();
  testlines[ttp++] = l;
}
#define ST(x) savetime(x);
#else
#define ST(x)
#endif

static char *monthnames[] = { "NULL", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
// line one of the button labels
static char *blabels1[] = { "<", "<", " ", ">", ">" };
// line two of the button labels
static char *blabels2[] = { " year", "month", "  now", "month", " year" };
static int rmon = 0;
static int ryr = 0;

void appCalendar(void) {
int fday, nmon, bwidth, dwidth, dheight, row;
static int nowyear, nowmonth, nowday, changedate;
time_t tsec;
static struct tm tms, tm1;
char *mname, *hname, *bname;
int16_t x, y, x1, y1;

NewDate:
#if DBG
  ttp = 0;
#endif
  ST(__LINE__);
  tft->fillScreen(TFT_BLACK);		// clear screen
  tft->setTextColor(TFT_GREEN);
  tft->setTextSize(1);
  tft->setTextFont(2);
  // button width
  bwidth = tft->width() / NUMBUTTONS;
  // size of a "day" cell on the calendar:
  dwidth = tft->width() / 7;
  dheight = tft->height() / 9;
  // tft->drawFastHLine(0, tft->height() - (dheight << 1), tft->width(), TFT_WHITE);
  tft->setTextColor(TFT_WHITE);
  for(int i = 0 ; i < NUMBUTTONS ; i++) {
    tft->fillRoundRect(i * (tft->width() / NUMBUTTONS), tft->height() - (dheight << 1), bwidth - 2, dheight << 1, 6, TFT_BLUE);
    tft->setCursor((bwidth >> 1) - 5 + (i * tft->width()) / NUMBUTTONS, tft->height() - ((6 * dheight) / 4));
    tft->print(blabels1[i]);
    tft->setCursor(5 + (i * tft->width()) / NUMBUTTONS, tft->height() - ((4 * dheight) / 4));
    tft->print(blabels2[i]);
    // tft->drawFastVLine(i * (tft->width() / NUMBUTTONS), tft->height() - (dheight << 1), dheight << 1, TFT_WHITE);
  }
  ST(__LINE__);
  tft->setTextColor(TFT_GREEN);
  tft->setCursor(0, 0);
  tft->setTextFont(4);
  ST(__LINE__);
  if(rmon == 0 || ryr == 0 || nowday == 0) {
    get_time_in_tz(tzindex);
    tms.tm_year = yyear - 1900;
    tms.tm_mon  = mmonth - 1;
    tms.tm_mday = dday;
    tms.tm_hour = hh;
    tms.tm_min  = mm;
    tms.tm_sec  = ss;
    ryr = yyear;
    rmon = mmonth;
    nowmonth = rmon;
    nowyear = ryr;
    nowday = tms.tm_mday;
#if DBG
    Serial.printf("INIT: %d/%02d/%02d %02d:%02d:%02d\n", ryr, rmon, nowday, tms.tm_hour, tms.tm_min, tms.tm_sec);
#endif
  }
  int need_mktime = 0;
  if(tms.tm_mon != rmon - 1
  || tms.tm_year != ryr - 1900) {
    need_mktime++;
  }
  tms.tm_year = ryr - 1900;
  tms.tm_mon = rmon - 1;
  tms.tm_mday = nowday;
  ST(__LINE__);
  if(need_mktime) {
    mktime(&tms);
  }
  ST(__LINE__);
  mname = monthnames[rmon];
  tm1 = tms;
  tm1.tm_mday = 1;
  mktime(&tm1);
#if DBG
  Serial.printf("ryr = %d, rmon = %d, tsec = %ld, nowday = %d, mname = %s\n",
    ryr, rmon, tsec, nowday, mname);
  Serial.printf("%dth day of this month is on day of week %d\n",
    tms.tm_mday, tms.tm_wday);
  Serial.printf("%dst day of this month is on day of week %d\n",
    tm1.tm_mday, tm1.tm_wday);
#endif
  ST(__LINE__);
  fday = tm1.tm_wday;
  sprintf(buff, "%s %d\n", mname, ryr);
  tft->drawCentreString(buff, tft->width() >> 1, 0, 4);
  row = 1;
  int j = fday;
  for(int i = 1 ; i <= 31 ; i++) {
    if(i > 28) {
      tm1.tm_mday = i;
      mktime(&tm1);
      nmon = tm1.tm_mon + 1;
#if DBG
      Serial.printf("rmon = %d, nmon = %d\n", rmon, nmon);
#endif
    }
    else {
      nmon = rmon;
    }
    if(rmon == nmon) {
      hname = is_holiday(rmon, i, j);
      bname = is_birthday(rmon, i, j);
      if(i == nowday && rmon == nowmonth && ryr == nowyear) {
	tft->setTextColor(TFT_RED);
      }
      else if(hname) {
	tft->setTextColor(TFT_YELLOW);
	Serial.printf("holiday %s on month %d, day %d, dow %d\n", hname, rmon, i, j);
      }
      else if(bname) {
	tft->setTextColor(TFT_CYAN);
	Serial.printf("birthday %s on month %d, day %d, dow %d\n", bname, rmon, i, j);
      }
      else {
	tft->setTextColor(TFT_GREEN);
      }
      sprintf(buff, "%2d", i);
      tft->setCursor(dwidth * j, row * dheight);
      tft->print(buff);
      if(j % 7 == 6) {
	row++;
      }
    }
    j++;
    j %= 7;
  }
  ST(__LINE__);
#if DBG
  long ttbase = testtimes[0];
  for(int i = 1 ; i < ttp ; i++) {
    if(testtimes[i]) {
      Serial.printf("line %3d, %ld msec\n", testlines[i], testtimes[i] - ttbase);
    }
  }
  memset(&testtimes[0], '\0', sizeof(testtimes)/sizeof(char));
#endif
  changedate = 0;
  while (!ttgo->getTouch(x, y)) { // Wait for touch
    my_idle();
  }
#if DBG
  Serial.printf("y = %d, ", y);
  Serial.printf("x = %d\n", x);
#endif
  if(x > 0
  && x < tft->width()
  && y > tft->height() - (dheight << 1)
  && y < tft->height()) {
#if DBG
    Serial.printf("y is on the buttons = %d, ", y);
    Serial.printf("x is %d\n", x);
#endif
    if(x < (tft->width() / NUMBUTTONS)) {
#if DBG
      Serial.printf("DEC YR: ryr was %d, ", ryr);
#endif
      ryr--;
#if DBG
      Serial.printf("ryr = %d\n", ryr);
#endif
    }
    else if(x < (2 * tft->width()) / NUMBUTTONS) {
#if DBG
      Serial.printf("DEC MON: rmon was %d, ", rmon);
#endif
      rmon--;
      if(rmon < 1) {
	ryr--;
	rmon = 12;
      }
#if DBG
      Serial.printf("rmon = %d, ryr = %d\n", rmon, ryr);
#endif
    }
    else if(x < (3 * tft->width()) / NUMBUTTONS) {
      ryr = nowyear;
      rmon = nowmonth;
    }
    else if(x < (4 * tft->width()) / NUMBUTTONS) {
      rmon++;
      if(rmon > 12) {
	ryr++;
	rmon = 1;
      }
    }
    else {
#if DBG
      Serial.printf("INC YR: ryr was %d, ", ryr);
#endif
      ryr++;
#if DBG
      Serial.printf("ryr = %d\n", ryr);
#endif
    }
    changedate++;
  }
  while (ttgo->getTouch(x, y)) { // wait until user lifts finger
    my_idle();
  }
  if(changedate) goto NewDate;
  
  tft->fillScreen(TFT_BLACK); // Clear screen
  tft->setTextSize(1);
  tft->setTextFont(1);
}
