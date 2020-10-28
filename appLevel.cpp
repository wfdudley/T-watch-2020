// Show the accelerometer working == "bubble level"
// derived from Dan Geiger's accelerometer demo

#include "config.h"
#include "DudleyWatch.h"

void appLevel() {
  ttgo->bma->begin();
  ttgo->bma->enableAccel();
  tft->fillScreen(TFT_BLACK);
  int16_t x, y;
  int16_t xpos, ypos;

  Accel acc;

  while (!ttgo->getTouch(x, y)) { // Wait for touch to exit

    tft->drawCircle(half_width, 120, 25, TFT_GREEN);
    tft->drawCircle(half_width, 120, 119, TFT_GREEN);
    tft->drawLine(0, 120, 239, 120, TFT_GREEN);
    tft->drawLine(120, 0, 120, 239, TFT_GREEN);
    ttgo->bma->getAccel(acc);
    xpos = acc.x;
    ypos = acc.y;
    tft->fillCircle(xpos / 10 + 119, ypos / 10 + 119, 10, TFT_RED); // draw dot
    delay(100);
    tft->fillCircle(xpos / 10 + 119, ypos / 10 + 119, 10, TFT_BLACK); // erase previous dot
    my_idle();
  }

  while (ttgo->getTouch(x, y)) { // wait user lifts finger to return to clock
    my_idle();
  }
  
  tft->fillScreen(TFT_BLACK); // Clear screen
}
