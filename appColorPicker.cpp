#include "config.h"
#include "DudleyWatch.h"
// pick a color in 565 color space
// Will display the color where you touch with 8 bits for each of r,g,b
// touch bottom left 1/6 of screen to exit
// this is modeled after the color palette used in the Gimp color selector,
// but the implementation was clean-sheet.

#define DBGPICKER 0

#define RED_EDGE 20
uint16_t cred(int x, int y) {
uint16_t red, d;
    // d = x - RED_EDGE;
    // Serial.printf("cred:   d = %d\n", d);
    if(x > RED_EDGE) { red = ((x - RED_EDGE) * 255)/(239 - RED_EDGE); }
    else { red = 0; }
    if(red > 255) { red = 255; }
    return red;
}

uint16_t cgreen(int x, int y) {
uint16_t green;
float d;
  d = (-.5774 * (float)x - 1 * (float)y + 335.48) / sqrt(-.5774 * -.5774 + -1 * -1);
  // Serial.printf("cgreen: d = %.2f\n", d);
  if(d < 0.0) { green = 0; }
  else {
    green = (255 * d) / 290;
  }
  if(green > 255) { green = 255; }
  return green;
}

uint16_t cblue(int x, int y) {
uint16_t blue;
float d;
  d = (.5774 * (float)x - 1 * (float)y - 95.48) / sqrt(.5774 * .5774 + -1 * -1);
  // Serial.printf("cblue:  d = %.2f\n", d);
  if(d > 0.0) { blue = 0; }
  else {
    blue = (255 * (0.0 - d)) / 290;
  }
  if(blue > 255) { blue = 255; }
  return blue;
}

void appColorPicker(void) {
  uint32_t endTime = millis() + 15000; // Timeout at 120 seconds
  int16_t x, y, lx, ly;
  lx = ly = -100;
  tft->fillScreen(TFT_BLACK);
  ttgo->getTouch(x, y); // get the last touch
  uint16_t color;
  uint16_t r, g, b;
  for(int y1 = 0 ; y1 < 240 ; y1++) {
    for(int x1 = 0 ; x1 < 240 ; x1++) {
      r = cred(x1, y1);
      g = cgreen(x1, y1);
      b = cblue(x1, y1);
      color = tft->color565(r, g, b);
#if DBGPICKER
      // print the values at the four corners
      // if((x1 == 0 && y1 == 239 ) || (x1 == 239 && y1 == 0 ) || (x1 == 239 && y1 == 239 ) || (x1 == 0 && y1 == 0 ))
      // print some values on a row through the center of the display
      // if(y1 == 120 && (x1 == 0 || x1 == RED_EDGE || x1 == 130 || x1 == 239))
      // print values in a column down the middle of the display
      if(x1 == 120 && ((y1 % 10) == 0))
      {
	Serial.printf("%3d, %3d, rgb = %3d, %3d, %3d, %04x\n", x1, y1, r, g, b, color);
      }
#endif
      tft->drawPixel(x1, y1, color);
    }
  }
  tft->drawLine(0, 200, 40, 200, TFT_DARKGREY);
  tft->drawLine(40, 200, 40, 239, TFT_DARKGREY);

  // while (endTime > millis())
  while (1)
  {
    my_idle();
    ttgo->getTouch(x, y);
    if(y > 200 && x < 40) { goto Leave; }
    // sometimes getTouch returns (0,0) even when it shouldn't, so test for that
    if(x && y && (x != lx || y != ly)) {
#if 1
      r = cred(x, y);
      g = cgreen(x, y);
      b = cblue(x, y);
      color = tft->color565(r, g, b);
#else
      color = tft->readPixel(x, y);	// doesn't work on T-watch.  Why?
#define NORMALIZED_TO_256 1
#if NORMALIZED_TO_256
#define SHIFT_MORE 3
#else
#define SHIFT_MORE 0
#endif
      b = (color & 0x001f) << (3 + SHIFT_MORE);
      g = (color & 0x03e0) >> (2 + SHIFT_MORE);
      r = (color & 0xf100) >> (8 + SHIFT_MORE);
#endif
#if DBGPICKER
      Serial.printf("x %3d, y %3d, color %4x, rgb = %3d,%3d,%3d\n", x, y, color, r, g, b);
#endif
      tft->fillRect(77, 98, 42, 37, TFT_BLACK);
      tft->setCursor(80, 100);
      tft->printf("R: %3d", r);
      tft->setCursor(80, 112);
      tft->printf("G: %3d", g);
      tft->setCursor(80, 124);
      tft->printf("B: %3d", b);
      lx = x;
      ly = y;
    }
  }
Leave:	;
  while (ttgo->getTouch(x, y)) {}  // Wait for release to exit
  tft->fillScreen(TFT_BLACK);
}
