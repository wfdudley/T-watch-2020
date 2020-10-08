// The Analog Clock "skin"
// derived from the TFTeSPI library, which has credits:
//    Gilchrist 6/2/2014 1.0
//    Update by Bodmer

#include <time.h>
#include <soc/rtc.h>
#include "config.h"
#include "DudleyWatch.h"
#include "my_tz.h"

#define BACKGROUND TFT_BLACK
#define LIGHT_WATCH_FACE 1
#if LIGHT_WATCH_FACE
// #define WATCH_FACE_COLOR TFT_DARKGREY
// #define WATCH_FACE_COLOR 0x4208		// super duper dark grey
#define WATCH_FACE_COLOR 0x2104		// really really dark grey
#define HANDS_COLOR TFT_WHITE
#define SECOND_HAND_COLOR TFT_RED
#else
#define WATCH_FACE_COLOR TFT_BLACK
#define HANDS_COLOR TFT_WHITE
#define SECOND_HAND_COLOR TFT_RED
#endif

#define CLOCK_CENTER_X 120	// was 65
#define CLOCK_CENTER_Y 108	// was 65
#define CLOCK_RADIUS 90.0	// was 61, then 105
#define HOUR_HAND_LENGTH (CLOCK_RADIUS * 33.0/61.0)
#define MINUTE_HAND_LENGTH (CLOCK_RADIUS * 44.0/61.0)
#define SECOND_HAND_LENGTH (CLOCK_RADIUS * 55.0/61.0)
#define HAND_WIDTH 3.0

// Saved H, M, S x & y multipliers :
static float sx = 0.0, sy = 1.0, mx = 1.0, my = 0.0, hx = -1.0, hy = 0.0;
static float hx1 = 0.0, hy1 = 1.0, hx2 = 0.0, hy2 = 1.0;
static float hx3 = 0.0, hy3 = 1.0, hx4 = 0.0, hy4 = 1.0;
static float mx1 = 0.0, my1 = 1.0, mx2 = 0.0, my2 = 1.0;
static float mx3 = 0.0, my3 = 1.0, mx4 = 0.0, my4 = 1.0;
static float sdeg=0.0, mdeg=0.0, hdeg=0.0;
static uint16_t osx  = CLOCK_CENTER_X - 1,
		osy  = CLOCK_CENTER_Y - 1,
		omx  = CLOCK_CENTER_X - 1,
		omy  = CLOCK_CENTER_Y - 1,
		omx1 = CLOCK_CENTER_X - 1,
		omy1 = CLOCK_CENTER_Y - 1,
		omx2 = CLOCK_CENTER_X - 1,
		omy2 = CLOCK_CENTER_Y - 1,
		omx3 = CLOCK_CENTER_X - 1,
		omy3 = CLOCK_CENTER_Y - 1,
		omx4 = CLOCK_CENTER_X - 1,
		omy4 = CLOCK_CENTER_Y - 1,
		ohx  = CLOCK_CENTER_X - 1,
		ohy  = CLOCK_CENTER_Y - 1,
		ohx1 = CLOCK_CENTER_X - 1,
		ohy1 = CLOCK_CENTER_Y - 1,
		ohx2 = CLOCK_CENTER_X - 1,
		ohy2 = CLOCK_CENTER_Y - 1,
		ohx3 = CLOCK_CENTER_X - 1,
		ohy3 = CLOCK_CENTER_Y - 1,
		ohx4 = CLOCK_CENTER_X - 1,
		ohy4 = CLOCK_CENTER_Y - 1;  // Saved H, M, S x & y coords
static uint16_t x0=0, x1=0, yy0=0, yy1=0;

void draw_clock_face(void) {
  tft->fillScreen(BACKGROUND);
  tft->setTextColor(TFT_RED, BACKGROUND);  // Adding a black background colour erases previous text automatically

  // Draw clock face
  tft->fillCircle(CLOCK_CENTER_X - 1, CLOCK_CENTER_Y - 1, CLOCK_RADIUS, TFT_BLUE);
  tft->fillCircle(CLOCK_CENTER_X - 1, CLOCK_CENTER_Y - 1, CLOCK_RADIUS-4, WATCH_FACE_COLOR);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx* (CLOCK_RADIUS - 4) + (CLOCK_CENTER_X - 1);
    yy0 = sy* (CLOCK_RADIUS - 4) + (CLOCK_CENTER_Y - 1);
    x1 = sx* (CLOCK_RADIUS - 11) + (CLOCK_CENTER_X - 1);
    yy1 = sy* (CLOCK_RADIUS - 11) + (CLOCK_CENTER_Y - 1);

    tft->drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }

// Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx* (CLOCK_RADIUS - 8) + (CLOCK_CENTER_X - 1);
    yy0 = sy* (CLOCK_RADIUS - 8) + (CLOCK_CENTER_Y - 1);
    
    tft->drawPixel(x0, yy0, TFT_BLUE);
    if(i==0 || i==180) tft->fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==0 || i==180) tft->fillCircle(x0+1, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft->fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft->fillCircle(x0+1, yy0, 1, TFT_CYAN);
  }
  tft->fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, 3, TFT_RED);
}

void draw_time_below_clock(void) {
char cbuf[20];
int h12;
  h12 = cvt_12_hour_clock(hh);
  sprintf(cbuf, "%02d:%02d %s", h12, mm,
    (!general_config.twelve_hr_clock) ? "  " : (hh < 12) ? "AM" : "PM");
#if 1
  // tft->fillRect(0, 220, 65, 23, BACKGROUND);
  tft->setTextSize(1);
  tft->setTextColor(TFT_RED, BACKGROUND);
  tft->drawString(cbuf, 0, 220, 4);
#else
  tft->setCursor(0, 210);
  tft->setTextFont(2);
  tft->setTextSize(2);
  tft->setTextColor(TFT_BLUE, BACKGROUND);
  tft->print(cbuf);
#endif
}

static void draw_date_below_clock(void) {
  tft->setTextSize(1);
  // tft->fillRect(240-130, 220, 130, 23, BACKGROUND);
  sprintf(buff, "%4d/%02d/%02d", yyear, mmonth, dday);
  tft->setTextColor(TFT_RED, BACKGROUND);
  tft->drawRightString(buff, 239, 220, 4);
  date_is_up = true;
}

void draw_step_counter_ljust (uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor, uint8_t textsize) {
    if(general_config.stepcounter_filter
    && ((step_counter != last_step_counter) || !steps_is_up)) {
      tft->setTextSize(textsize);
      tft->setTextColor(fgcolor, bgcolor);
      tft->setCursor(ox, oy);
      tft->setTextSize(1);
      tft->setTextFont(4);
      tft->print(step_counter);
      // tft->print(" ");
      steps_icon(tft->getCursorX(), (oy > 3) ? oy-3 : 0, fgcolor, bgcolor, (textsize > 2) ? 2 : 1);
      last_step_counter = step_counter;
      steps_is_up = true;
    }
}

void update_clock_face(void) {
int h12;
  // Pre-compute hand degrees, x & y coords for a fast screen update
  // Serial.printf("ss = %d\n", ss);
  sdeg = ss * 6;                  // 0-59 -> 0-354
  sx  = cos((sdeg-90)*0.0174532925);    
  sy  = sin((sdeg-90)*0.0174532925);

  if (ss == 0 || initialAnalog) {	// minute change
    // Serial.println(F("new minute"));
    if(mm == 0 || initialAnalog) {	// hour change
      // Serial.println(F("new hour"));
      h12 = hh % 12;
      hdeg = h12 * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - incl. min and sec

      hx  = cos((hdeg -  90) * 0.0174532925);    
      hy  = sin((hdeg -  90) * 0.0174532925);
      hx1 = cos((hdeg      ) * 0.0174532925);    
      hy1 = sin((hdeg      ) * 0.0174532925);
      hx2 = cos((hdeg - 180) * 0.0174532925);    
      hy2 = sin((hdeg - 180) * 0.0174532925);
      // erase hour hand
      tft->drawLine(ohx, ohy, CLOCK_CENTER_X, CLOCK_CENTER_Y, WATCH_FACE_COLOR);
      tft->drawLine(ohx1, ohy1, ohx, ohy, WATCH_FACE_COLOR);
      tft->drawLine(ohx2, ohy2, ohx, ohy, WATCH_FACE_COLOR);
      tft->drawLine(ohx3, ohy3, ohx, ohy, WATCH_FACE_COLOR);
      tft->drawLine(ohx4, ohy4, ohx, ohy, WATCH_FACE_COLOR);

      // recompute hour hand
      ohx  = hx*HOUR_HAND_LENGTH + CLOCK_CENTER_X;
      ohy  = hy*HOUR_HAND_LENGTH + CLOCK_CENTER_Y;
      ohx1 = hx1*HAND_WIDTH + CLOCK_CENTER_X;
      ohy1 = hy1*HAND_WIDTH + CLOCK_CENTER_Y;
      ohx2 = hx2*HAND_WIDTH + CLOCK_CENTER_X;
      ohy2 = hy2*HAND_WIDTH + CLOCK_CENTER_Y;
      ohx3 = hx1*(HAND_WIDTH-1) + CLOCK_CENTER_X;
      ohy3 = hy1*(HAND_WIDTH-1) + CLOCK_CENTER_Y;
      ohx4 = hx2*(HAND_WIDTH-1) + CLOCK_CENTER_X;
      ohy4 = hy2*(HAND_WIDTH-1) + CLOCK_CENTER_Y;

    }
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds contrib.
    mx  = cos((mdeg- 90) * 0.0174532925);    
    my  = sin((mdeg- 90) * 0.0174532925);
    mx1 = cos((mdeg    ) * 0.0174532925);    
    my1 = sin((mdeg    ) * 0.0174532925);
    mx2 = cos((mdeg-180) * 0.0174532925);    
    my2 = sin((mdeg-180) * 0.0174532925);

    // erase minute hand
    tft->drawLine( omx,  omy, CLOCK_CENTER_X, CLOCK_CENTER_Y, WATCH_FACE_COLOR);
    tft->drawLine(omx1, omy1, omx, omy, WATCH_FACE_COLOR);
    tft->drawLine(omx2, omy2, omx, omy, WATCH_FACE_COLOR);
    tft->drawLine(omx3, omy3, omx, omy, WATCH_FACE_COLOR);
    tft->drawLine(omx4, omy4, omx, omy, WATCH_FACE_COLOR);

    // recompute minute hand
    omx  = mx * MINUTE_HAND_LENGTH + CLOCK_CENTER_X;    
    omy  = my * MINUTE_HAND_LENGTH + CLOCK_CENTER_Y;
    omx1 = mx1*3 + CLOCK_CENTER_X;    
    omy1 = my1*3 + CLOCK_CENTER_Y;
    omx2 = mx2*3 + CLOCK_CENTER_X;    
    omy2 = my2*3 + CLOCK_CENTER_Y;
    omx3 = mx1*2 + CLOCK_CENTER_X;    
    omy3 = my1*2 + CLOCK_CENTER_Y;
    omx4 = mx2*2 + CLOCK_CENTER_X;    
    omy4 = my2*2 + CLOCK_CENTER_Y;

    initialAnalog = false;
  }
  // draw new minute hand
  tft->drawLine(omx, omy, CLOCK_CENTER_X, CLOCK_CENTER_Y, HANDS_COLOR);
  tft->drawLine(omx1, omy1, omx, omy, HANDS_COLOR);
  tft->drawLine(omx2, omy2, omx, omy, HANDS_COLOR);
  tft->drawLine(omx3, omy3, omx, omy, HANDS_COLOR);
  tft->drawLine(omx4, omy4, omx, omy, HANDS_COLOR);

  // draw new hour hand
  tft->drawLine( ohx,  ohy, CLOCK_CENTER_X, CLOCK_CENTER_Y, HANDS_COLOR);
  tft->drawLine(ohx1, ohy1, ohx, ohy, HANDS_COLOR);
  tft->drawLine(ohx2, ohy2, ohx, ohy, HANDS_COLOR);
  tft->drawLine(ohx3, ohy3, ohx, ohy, HANDS_COLOR);
  tft->drawLine(ohx4, ohy4, ohx, ohy, HANDS_COLOR);

  // erase old second hand
  tft->drawLine(osx, osy, CLOCK_CENTER_X, CLOCK_CENTER_Y, WATCH_FACE_COLOR);

  // draw new second hand:
  osx = sx*SECOND_HAND_LENGTH + CLOCK_CENTER_X;    
  osy = sy*SECOND_HAND_LENGTH + CLOCK_CENTER_Y;
  tft->drawLine(osx, osy, CLOCK_CENTER_X, CLOCK_CENTER_Y, SECOND_HAND_COLOR);
  // we remember value of osx and osy until the next time around

  // "dot" on the axis of the hand.
  tft->fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, 3, SECOND_HAND_COLOR);
}

void Analog_Time(uint8_t fullUpdate) {
  // Get the current time and date
  get_time_in_tz(tzindex);
  local_hour = hh;
  local_minute = mm;

  tft->setTextSize(1);
  if(fullUpdate == 2) {
    initialAnalog = true;
    draw_clock_face();
    if(if_not_home_tz()) {
      tft->setTextColor(TFT_RED, BACKGROUND);
      sprintf(buff, "%02d:%02d @home", home_hh, home_mm);
      tft->drawCentreString(buff, 0, 195, 4);
    }
    if(!date_is_up || initialAnalog) {
      draw_time_below_clock();
      draw_date_below_clock();
    }
  }
  else if(fullUpdate == 1) {
    draw_date_below_clock();
  }
  else {
    update_clock_face();
    draw_time_below_clock();
    draw_step_counter_ljust(0, 0, TFT_RED, BACKGROUND, 2);
    // draw_step_counter_ljust(0, 195, TFT_RED, BACKGROUND, 2);
    charge_icon(215, 20, TFT_RED, TFT_BLACK, false);
  }

  if (fullUpdate) {
    // show battery percentage
    int per;
    do {
      per = power->getBattPercentage();
    } while(per > 100);
    tft->setTextSize(1);
    sprintf(buff, "%d%%", per);
    tft->setTextColor(TFT_RED, BACKGROUND);
    tft->drawRightString(buff, 239, 195, 4);
    battery_icon(190, 1, TFT_RED, TFT_BLACK);
  }
}
