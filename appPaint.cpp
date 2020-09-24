// "swiping" test / paint program
#include "config.h"
#include "DudleyWatch.h"
#include <math.h>

void appPaint(void) {
uint32_t lasttouch, interval;
int16_t x, y, x0, y0;
enum SWIPE_DIR swipe;

  ttgo->tft->fillScreen(TFT_BLACK);
  lasttouch = millis();
  x0 = y0 = -1;
  while (1) {
    interval = millis() - lasttouch;
    my_idle();
    if(ttgo->getTouch(x, y)) {
      if((x > 0) && (y > 0) && (x < 240) && (y < 240)) {
	if(interval < 50) {
	  if(x0 >= 0 && y0 >= 0) {
	    ttgo->tft->drawLine(x0, y0, x, y, TFT_RED);
	  }
	}
	x0 = x;
	y0 = y;
      }
      lasttouch = millis();
    }
    else if(interval >= 10000) {
      // Serial.println(F("10 seconds, no touch"));
      break;
    }
  }
  // Serial.println(F("outside while(1), exit appPaint()"));
  ttgo->tft->fillScreen(TFT_BLACK);
}
