// Dan Geiger: Display Jupiters 4 moons
// It uses the current date time and corrects to get UTC time
// Make sure you set the correct Time Zone below

// modified by W.F.Dudley Jr.
// no longer need to correct to UTC time as we keep the rtc in UTC time

#include <soc/rtc.h>
#include "config.h"
#include "DudleyWatch.h"

void jSats() {
uint32_t updatetime;
int16_t x, y;
int16_t iX1, iX2, iX3, iX4, liX1, liX2, liX3, liX4;
  updatetime = 0;

  liX1 = liX2 = liX3 = liX4 = 0;
  do {
    if(updatetime + 10000 < millis()) {
      updatetime = millis();
      // Get the current info
      RTC_Date tnow = ttgo->rtc->getDateTime();

      hh = tnow.hour;
      mm = tnow.minute;
      ss = tnow.second;
      dday = tnow.day;
      mmonth = tnow.month;
      yyear = tnow.year;
      Serial.printf("rtc -> %d/%02d/%02d %d:%02d:%02d\n", yyear, mmonth, dday, hh, mm, ss);

      float tDay = dday; // Calculate the day plus fractional day
      tDay += (float)hh / 24.0;
      tDay += (float)mm / 1440.0;
      tDay += (float)ss / (60.0 * 1440.0);
      int16_t tYear = yyear;
      int8_t tMonth = mmonth;

      // Calculate the Julian Date offset from Epoch
      int16_t cYear, cMonth;
      if (tMonth < 3) {
	cYear = tYear - 1;
	cMonth = tMonth + 12;
      }
      else {
	cYear = tYear;
	cMonth = tMonth;
      }
      int a = cYear / 100;
      int b = 2 - a + (int)(a / 4);
      long c = 365.25 * cYear;
      long d = 30.6001 * (cMonth + 1);
      float N = b + c + d + tDay - 694025.5;
      // N is Julian Date

      // Calc moon positions
      float P = PI / 180;
      float MT = (358.476 + 0.9856003 * N) * P;
      float MJ = (225.328 + 0.0830853 * N) * P;
      float JJ = 221.647 + 0.9025179 * N;
      float VT = 1.92 * sin(MT) + 0.02 * sin(2 * MT);
      float VJ = 5.55 * sin(MJ) + 0.17 * sin(2 * MJ);
      float K = (JJ + VT - VJ) * P;
      float DT = sqrt(28.07 - 10.406 * cos(K));
      float Z1 = sin(K) / DT;
      float I = atan(Z1 / sqrt(1 - Z1 * Z1));
      I = I / P;
      float F = (N - DT / 173);
      float F1 = I - VJ;
      float U1 = 84.5506 + 203.405863 * F + F1;
      float U2 = 41.5015 + 101.2916323 * F + F1;
      float U3 = 109.9770 + 50.2345169 * F + F1;
      float U4 = 176.3586 + 21.4879802 * F + F1;
      float X1 = 5.906 * sin(U1 * P);
      float X2 = 9.397 * sin(U2 * P);
      float X3 = 14.989 * sin(U3 * P);
      float X4 = 26.364 * sin(U4 * P);
      iX1 = X1 * 4;
      iX2 = X2 * 4;
      iX3 = X3 * 4;
      iX4 = X4 * 4;
      // Print out results

      // only refresh the screen when there's a one pixel movement.
      if(liX1 != iX1 || liX2 != iX2 || liX3 != iX3 || liX4 != iX4) {
	Serial.println(F("redraw"));
	tft->setTextFont(1);
	ttgo->tft->fillScreen(TFT_BLACK);
	ttgo->tft->setTextSize(2);
	ttgo->tft->setCursor( 0, 10);
	ttgo->tft->setTextColor(TFT_ORANGE);
	ttgo->tft->print(" IO:       ");
	ttgo->tft->print(X1, 1);
	ttgo->tft->setCursor( 0, 30);
	ttgo->tft->setTextColor(TFT_BLUE);
	ttgo->tft->print(" EUROPA:   ");
	ttgo->tft->println(X2, 1);
	ttgo->tft->setCursor( 0, 50);
	ttgo->tft->setTextColor(TFT_GREEN);
	ttgo->tft->print(" GANYMEDE: ");
	ttgo->tft->println(X3, 1);
	ttgo->tft->setCursor( 0, 70);
	ttgo->tft->setTextColor(TFT_YELLOW);
	ttgo->tft->print(" CALLISTO: ");
	ttgo->tft->println(X4, 1);

	//Now display them as they would appear

	ttgo->tft->fillCircle(119, 155, 6, TFT_RED); // Jupiter

	ttgo->tft->setTextColor(TFT_ORANGE);
	ttgo->tft->fillCircle(int(X1 * 4 + 119), 155, 2, TFT_ORANGE);
	ttgo->tft->drawString("I", int(X1 * 4 + 119)-3, 175 , 1);
	ttgo->tft->setTextColor(TFT_BLUE);
	ttgo->tft->fillCircle(int(X2 * 4 + 119), 155, 2, TFT_BLUE);
	ttgo->tft->drawString("E", int(X2 * 4 + 119)-3, 175, 1);
	ttgo->tft->setTextColor(TFT_GREEN);
	ttgo->tft->fillCircle(int(X3 * 4 + 119), 155, 2, TFT_GREEN);
	ttgo->tft->drawString("G", int(X3 * 4 + 119)-3, 175, 1);
	ttgo->tft->setTextColor(TFT_YELLOW);
	ttgo->tft->fillCircle(int(X4 * 4 + 119), 155, 2, TFT_YELLOW);
	ttgo->tft->drawString("C", int(X4 * 4 + 119)-3, 175, 1);
	liX1 = iX1;
	liX2 = iX2;
	liX3 = iX3;
	liX4 = iX4;
      }
    }
    my_idle();
  } while (!ttgo->getTouch(x, y));
  while (ttgo->getTouch(x, y)) {	// Wait for release
    my_idle();
  }
  ttgo->tft->fillScreen(TFT_BLACK);
}
