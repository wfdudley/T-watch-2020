// stopwatch
// W.F.Dudley Jr.

#include "config.h"
#include "DudleyWatch.h"

static struct menu_item swatch_menu[]
  = {
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "",           "", NULL},
      { "Run/Stop",   "", NULL},
      { "Clear",      "", NULL},
      { "Exit",       "", NULL}
    };

uint8_t eh, em, es;
uint16_t ems;
uint32_t start_time, et, et_last;
boolean running;
boolean has_run;
boolean is_cleared;

void clear_time(void) {
  et = 0;
  et_last = 0;
  is_cleared = true;
}

void start_clock(void) {
  running = true;
  has_run = true;
  start_time = millis();
  if(is_cleared) {
    is_cleared = false;
  }
}

void appStopWatch(void) {
int mSelect;
  running = false;
  has_run = false;
  is_cleared = true;

  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->drawCentreString("STOP WATCH",  half_width, 10, 2);
  tft->setTextColor(TFT_ORANGE, TFT_BLACK);
  // tft->setTextFont(2);
  // put button(s) on the screen
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 0, NULL, true, 0);
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 1, NULL, true, 0);
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 2, NULL, true, 0);

  while(1) {
    mSelect = poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
//  if(mSelect > -1) {
//    Serial.printf("mSelect = %d\n", mSelect);
//  }
    if (mSelect >= 9 && mSelect <= 11) {	// if user touched a button
      int col = mSelect % 3;
      int row = mSelect / 3;
      flash_menu_item(3, swatch_menu, 1, 2, true, row, col, NULL, true, 0);
      if(col == 1) {	// clear button
	// Serial.println(F("clear"));
	clear_time();
      }
      else if(col == 2) {	// exit
	// Serial.println(F("exit"));
	goto Exit;
      }
      else {
	// Serial.println(F("start/stop"));
	if(running) {
	  // Serial.println(F("stop"));
	  running = false;
	  et_last = et;
	}
	else {
	  // Serial.println(F("run"));
	  start_clock();
	}
      }
    }
    // uint32_t now = millis();
    if(running) {
      et = et_last + millis() - start_time;
    }
    else if(!has_run) {
      et = 0;
    }
    ems = et % 1000;
    es = (et / 1000) % 60;
    em = (et / 60000) % 60;
    eh = (et / 3600000);
    sprintf(buff, "%d:%02d:%02d.%03d", eh, em, es, ems);
    tft->setTextColor(TFT_ORANGE, TFT_BLACK);
    tft->setTextSize(1);
    tft->drawString(buff, 50, 100, 4);
    delay(1);
    my_idle();
  }
Exit:
  tft->fillScreen(TFT_BLACK);
}
