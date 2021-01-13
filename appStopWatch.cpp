// stopwatch
// W.F.Dudley Jr.
//
// Added "Smart Sound" option ("HeliTimer") by Fra4prg
// "Smart Sound" enables you to hear the number of minutes without having to look on the display.
// The idea behind this is that human brain can identify the number of 3 beeps as triple without counting them
// even if they are very short and fast.
// So up to 3 beeps are played slowly and preceeding triples are faster.
// For every 10 minutes a sweep sound is preceeding.
// A new top left button switches modes between [no sound] - [vibration + sound] - [only sound]
// Some examples to show the sequence:
// Minute  1: C5
// Minute  2: C5-E5
// Minute  3: C5-E5-G5
// Minute  4: triple - C5
// Minute  5: triple - C5-E5
// ...
// Minute  9: triple - triple - C5-E5-G5
// Minute 10: sweep
// ...
// Minute 18: sweep - triple - triple - C5-E5
// and so on...
// As you can see this works well for short times up to 10 minutes then it gets weird...
// The idea came up when I was flying model helicopters where I had to control the flight time 
// due to small batteries and could not loose visual contact to my heli model.
// So I made a small Arduino based timer called HeliTimer.
// But this function fits perfect to the T-watch :-)

#include "config.h"
#include "DudleyWatch.h"

static struct menu_item swatch_menu[]
  = {
      { "",           "", NULL},
      { "",           "", NULL},
      { "Mode",       "", NULL},   // Fra4prg: new button for Smart Sound
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
uint8_t min_123, min_triple, min_tens, old_min_123; // Fra4prg: beep minute classification
uint8_t runmode; // Fra4prg: 0=no sound, 1=vib.+sound, 2=sound

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
  old_min_123 = 0;  // Fra4prg
  runmode = 0;  // Fra4prg

  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->drawCentreString("STOP WATCH",  half_width, 10, 2);
  tft->setTextColor(TFT_ORANGE, TFT_BLACK);
  // tft->setTextFont(2);
  // put button(s) on the screen

  // Fra4prg: new button (don't know how to usually change the lower text on the button)
  flash_menu_item(3, swatch_menu, 1, 2, true, 0, 2, NULL, true, 0);
  tft->fillRect(164, 50, 74, 20, TFT_BLUE);
  tft->setTextColor(TFT_YELLOW, TFT_BLUE);
  tft->drawCentreString("no sound", 200, 52, 2);
  // Fra4prg: end new button
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 0, NULL, true, 0);
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 1, NULL, true, 0);
  flash_menu_item(3, swatch_menu, 1, 2, true, 3, 2, NULL, true, 0);

  while(1) {
    mSelect = poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
//  if(mSelect > -1) {
//    Serial.printf("mSelect = %d\n", mSelect);
//  }
    // Fra4prg: new button (don't know how to usually change the lower text on the button)
    if (mSelect == 2) {  // if user touched mode button
      runmode++;
      if (runmode>2) {runmode=0;};
      tft->fillRect(164, 50, 74, 20, TFT_BLUE);
      switch (runmode) {
        case 0 :
          tft->setTextColor(TFT_YELLOW, TFT_BLUE);
          tft->drawCentreString(" No Sound ", 200, 52, 2);
          break;
        case 1 :
          tft->setTextColor(TFT_YELLOW, TFT_PURPLE);
          tft->drawCentreString("Vib.+Sound", 200, 52, 2);
          break;
        case 2 :
          tft->setTextColor(TFT_BLACK, TFT_GREEN);
          tft->drawCentreString(" Sound ", 200, 52, 2);
          break;
        default :
          tft->setTextColor(TFT_YELLOW, TFT_BLUE);
          tft->drawCentreString(" No Sound ", 200, 52, 2);
          runmode=0;
          break;
      }
    }
    // Fra4prg: end new button
    if (mSelect >= 9 && mSelect <= 11) {	// if user touched a button
      int col = mSelect % 3;
      int row = mSelect / 3;
      flash_menu_item(3, swatch_menu, 1, 2, true, row, col, NULL, true, 0);
      if(col == 1) {	// clear button
	// Serial.println(F("clear"));
	clear_time();
  old_min_123 = 0;  // Fra4prg
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
    
    // Fra4prg: begin beep smart sound
    if (runmode>0) {
      min_tens = (em - (em % 10))/10;
      if ( (em-(min_tens*10)) == 0 ) {
        min_123 = 3;
      }
      else {
        min_123 = ((em-1)-(10*min_tens)) % 3;
      }
      if ((em-(min_tens*10))==0) {
        min_triple = 0;
      }
      else {
        min_triple = (((em-min_tens*10)-min_123)-1)/3;     
      }
      // now play beeps:
      if (old_min_123!=min_123) {  // if new minute is detected
        old_min_123=min_123;
        if (runmode==1) {
          quickBuzz();
        }
        while(min_tens>0) {
          beep(13);
          min_tens--;
        }
        delay(50);
        while(min_triple>0) {
          beep(12);
          min_triple--;
        }
        delay(50);
        if(min_123<3) {
          beep(9);
        }
        if((min_123<3)&&(min_123>0)) {
          beep(10);
        }
        if(min_123==2) {
          beep(11);
        }
      }
    }
    // Fra4prg: end beep smart sound
    
    tft->setTextColor(TFT_ORANGE, TFT_BLACK);
    tft->setTextSize(1);
    tft->drawString(buff, 50, 100, 4);
    delay(1);
    my_idle();
  }
Exit:
  tft->fillScreen(TFT_BLACK);
}
