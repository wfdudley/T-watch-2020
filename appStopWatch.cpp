// stopwatch
// W.F.Dudley Jr.
//
// modified by Frank Scholl (Fra4prg) 31.01.2021
// added Smart Sound
// added Tea Timer Function
// completely reprogrammed as simple state machine
//
/* Added "Smart Sound" option ("HeliTimer") by Fra4prg 2021 01 13
   "Smart Sound" enables you to hear the number of minutes without having to look on the display.
   The idea behind this is that human brain can identify the number of 3 beeps as triple without counting them
   even if they are very short and fast.
   So up to 3 beeps are played slowly and preceeding triples are faster.
   For every 10 minutes a sweep sound is preceeding.
   A new top left button switches modes between [no sound] - [vibration + sound] - [only sound]
   Some examples to show the sequence:
   Minute  1: C5
   Minute  2: C5-E5
   Minute  3: C5-E5-G5
   Minute  4: triple - C5
   Minute  5: triple - C5-E5
   ...
   Minute  9: triple - triple - C5-E5-G5
   Minute 10: sweep
   ...
   Minute 18: sweep - triple - triple - C5-E5
   and so on...
   As you can see this works well for short times up to 10 minutes then it gets weird...
   The idea came up when I was flying model helicopters where I had to control the flight time due to small batteries and could not loose visual contact to my heli model.
   So I made a small Arduino based timer called HeliTimer.
   But this function fits perfect to the T-watch :-)
*/
/* Added "TeaTimer" option by Fra4prg 2021 01 31
   With the top left button you can switch between Stop Watch (counting up) and Tea Timer, counting down from a preset time. Set the time with new buttons plus and minus, switch between setting Minutes/Hours/Seconds with the Set button.

   When Tea Timer count down reaches zero then a melody is played to remember you to pull the tea bag out of your cup (or what else you were waiting for).

   The melody is played again ervery minute until you press the clear button. 
   Time is flashing between red and orange, also the Icon flashes between cup and bag.  --> It's time to pull the tea bag out of your cup!

   After 30 seconds of goodwill time the sum of time is shown,
*/
// Due to increased complexity I changed the structure into a simple state machine


#include "config.h"
#include "DudleyWatch.h"
#include "icons.h"

// debug infos on serial monitor
#define DBGSW 0

// Sound modes
#define SILENT    0
#define VIP_BEEP  1
#define BEEP      2

// setting hour / minute / seconds
#define SET_S 0
#define SET_M 1
#define SET_H 2

// clock icon colours
#define CLOCK_CASE TFT_DARKGREY
#define CLOCK_FACE TFT_NAVY
#define CLOCK_POINTER TFT_RED

// progress bar position
#define PROGBAR_Y 140
#define PROGBAR_HEIGHT 19

// states of FSM

#define Init_State        0
#define SW_cleared_State  1
#define SW_running_State  2
#define SW_paused_State   3
#define TT_cleared_State  4
#define TT_running_State  5
#define TT_paused_State   6
#define TT_SetTime_State  7
#define TT_end_State      8
#define Exit_state        9
#define Error_state      10

#define  Button_Mode     0
#define  Button_Sound    2
#define  Button_Set      6
#define  Button_Plus     7
#define  Button_Minus    8
#define  Button_RunStop  9
#define  Button_Clear    10
#define  Button_Exit     11
#define  No_Button       -1

int mButton; 

static struct menu_item swatch_menu[]
  = {
      { "Mode",         "", NULL},   // new button for Tea Timer Mode
      { "",             "", NULL},
      { "Sound",        "", NULL},   // new button for Smart Sound
      { "",             "", NULL},
      { "",             "", NULL},
      { "",             "", NULL},
      { "Set",          "", NULL},   // new button Set Tea Timer Start
      { "+",            "", NULL},
      { "-",            "", NULL},
      { "Run/Stop",     "", NULL},
      { "Clear",        "", NULL},
      { "Exit",         "", NULL}
    };

// clock pointer positions
uint8_t cp[9][4] {
    {120, 65, 112, 56},  // [0] 10h30
    {120, 65, 120, 54},  // [1] 12h00
    {120, 65, 128, 56},  // [2] 01h30
    {120, 65, 131, 65},  // [3] 03h00
    {120, 65, 128, 73},  // [4] 04h30
    {120, 65, 120, 76},  // [5] 06h00
    {120, 65, 112, 73},  // [6] 07h30
    {120, 65, 109, 65},  // [7] 09h00
    {120, 65, 112, 56}   // [8] 10h30
};

uint8_t eh, em, es;
uint16_t ems;
uint32_t start_time, et, et_last;   // start_time=millies() at start, et=time to display, et_last=tine at last stop
uint32_t ett, ettr, ettr_s, ett_ovr; // ett=Tea Timer Set Time
uint8_t min_123, min_triple, min_tens, old_min_123; // beep minute classification
uint8_t beepmode_sw;                // beepmode for stopwatch: 0=no sound, 1=vib.+sound, 2=sound
uint8_t beepmode_tt;                // beepmode for Te Timer: 0=no sound, 1=vib.+sound, 2=sound
uint8_t setmode;                    // SET_S = set seconds, SET_M = set minutess, SET_H = set hours 
uint32_t progbar;
uint8_t clock_pointer;              // position of clock pointer 0..7
uint8_t eNextState;                 // FSM state

void erase_set_buttons(void) {
  tft->fillRect(0, 135, 239, 45, TFT_BLACK); // erase set buttons
}

void erase_clear_button(void) {
  tft->fillRect(78, 185, 82, 45, TFT_BLACK); // erase clear button
}

void drawClockPointer(void) {
  tft->drawLine(cp [clock_pointer]  [0], cp [clock_pointer]  [1], cp [clock_pointer]  [2], cp [clock_pointer]  [3], CLOCK_FACE);    // erase old
  tft->drawLine(cp [clock_pointer+1][0], cp [clock_pointer+1][1], cp [clock_pointer+1][2], cp [clock_pointer+1][3], CLOCK_POINTER); // draw new
}

void play_sound(void) {   // if selected beep smart sound
  if (beepmode_sw>SILENT) {
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
      if (beepmode_sw==VIP_BEEP) {
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
}

void update_SOUND_button(uint8_t* beepmode, boolean change) {
  if (change) {
    (*beepmode)++;
    if (*beepmode > BEEP) { *beepmode=0; };
  }
  switch (*beepmode) {
    case SILENT :
      flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 2, "silent", true, 0);
      break;
    case VIP_BEEP :
      flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 2, "vib+beep", true, 0);
      break;
    case BEEP :
      flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 2, "beep", true, 0);
      break;
    default :
      break;
  }
}

void displayTime(uint16_t color_f, uint16_t color_b) {
#if DBGSW
  //Serial.printf("SW running et = %d, ett = %d\n", et, ett);
#endif
  ems = et % 1000;
  es = (et / 1000) % 60;
  em = (et / 60000) % 60;
  eh = (et / 3600000);

  tft->setTextColor(color_f, color_b);
  sprintf(buff, "%d:%02d:%02d.%03d", eh, em, es, ems);
  tft->setTextSize(1);
  tft->drawString(buff, 50, 100, 4);
  delay(1);
 
	my_idle();
}


// ***** eventhandlers *****

uint8_t Init_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered Init_Handler\n");
#endif
	beepmode_sw = SILENT;
	beepmode_tt = VIP_BEEP;
	setmode = SET_M;
	ett = 6 * 60000; // default Tea Timer Start = 6 Minutes
	ettr = 0;

	tft->fillScreen(TFT_BLACK);
//	tft->setTextColor(TFT_ORANGE, TFT_BLACK);

	// these buttons are always shown
//	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 0, "Stop Watch", true, 0); // button Mode
	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 2, "silent", true, 0); // button Sound
	flash_menu_item(3, swatch_menu, 1, 2, true, 3, 0, NULL, true, 0); // button run/stop
	flash_menu_item(3, swatch_menu, 1, 2, true, 3, 2, NULL, true, 0); // button Exit

	// draw clock face
	tft->fillCircle(120, 41, 3, CLOCK_CASE);  // 3 dots
	tft->fillCircle(108, 46, 3, CLOCK_CASE);
	tft->fillCircle(132, 46, 3, CLOCK_CASE);

	tft->fillCircle(120, 65, 22, CLOCK_CASE);
	tft->fillCircle(120, 65, 18, CLOCK_FACE);
	tft->fillRect(118, 42, 4, 3, CLOCK_CASE);

	clock_pointer = 0;
	drawClockPointer();

  return SW_cleared_State;
}

uint8_t SW_Cleared_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered SW_Cleared_Handler\n");
#endif

  // Headline
  tft->fillRect(0, 0, 239, 32, TFT_BLACK);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->drawCentreString("STOP WATCH",  half_width, 10, 2);

  // display SW specific buttons
  flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 0, "Stop Watch", true, 0); // button Mode
  update_SOUND_button( &beepmode_sw, false );
  erase_set_buttons();
  erase_clear_button();

	// reset variables
  et = 0;
  et_last = 0;
  old_min_123 = 0;

  // reset clock icon
  clock_pointer=0;
  tft->fillCircle(120, 65, 18, CLOCK_FACE);
	drawClockPointer();

	displayTime(TFT_ORANGE, TFT_BLACK);

	// main

	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_Mode :
				return TT_cleared_State;
				break;
			case Button_RunStop :
				start_time = millis();
				return SW_running_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_sw, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}
    mButton = No_Button;
	}
}

uint8_t SW_Running_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered SW_Running_Handler\n");
#endif
	erase_clear_button();

	// main
	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_RunStop :
				return SW_paused_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_sw, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}

		clock_pointer++;
		if (clock_pointer>=8) clock_pointer=0;
		drawClockPointer();

		et = et_last + millis() - start_time; // count up
		displayTime(TFT_ORANGE, TFT_BLACK);

		play_sound();

    mButton = No_Button;
	}
}

uint8_t SW_Paused_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered SW_Paused_Handler\n");
#endif
	et_last = et;
	flash_menu_item(3, swatch_menu, 1, 2, true, 3, 1, NULL, false, 0); // add button clear

	// main
	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_Clear :
				return SW_cleared_State;
				break;
			case Button_RunStop :
				start_time = millis();
				return SW_running_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_sw, true );
				break;
			case Button_Mode :
				return TT_cleared_State;
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}
		displayTime(TFT_ORANGE, TFT_BLACK);
    mButton = No_Button;
	}
}

uint8_t TT_Cleared_Handler(void) {
	// entry
#if DBGSW
  Serial.printf("entered TT_Cleared_Handler\n");
#endif

  // Headline
  tft->fillRect(0, 0, 239, 32, TFT_BLACK);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->drawCentreString("TEA TIMER",  half_width, 10, 2);

  tft->setSwapBytes(true);
  tft->pushImage(20, 0, 32, 32, tea_cup);

  // display TT specific buttons
  tft->fillRect(0, PROGBAR_Y, 239, PROGBAR_HEIGHT, TFT_BLACK); // erase progress bar
	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 0, 0, "Tea Timer", true, 0);
	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 0, "Minutes", false, 0);
	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 1, "plus", false, 0);
	flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 2, "minus", false, 0);          
	erase_clear_button();
  update_SOUND_button( &beepmode_tt, false );

  // reset clock icon
  clock_pointer=0;
  tft->fillCircle(120, 65, 18, CLOCK_FACE);
	drawClockPointer();

  // reset variables
  et = ett;
  et_last = ett;
	setmode = SET_M;

	// main

	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_Mode :
				return SW_cleared_State;
				break;
			case Button_RunStop :
        start_time = millis();
				return TT_running_State;
				break;
			case Button_Set :
				setmode++;
				if (setmode>SET_H) setmode=SET_S;  // M -> S -> H -> M ...
				switch (setmode) {
				  case SET_S :
					flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 0, "Seconds", false, 0);
					break;
				  case SET_M :
					flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 0, "Minutes", false, 0);
					break;
				  case SET_H :
					flash_menu_item_txt(3, swatch_menu, 1, 2, true, 2, 0, "Hours", false, 0);
					break;
				  default :
					break;
				}
				break;
			case Button_Plus :
				switch (setmode) {
					case SET_S :
					  et += 1000;
					  break;
					case SET_M :
					  et += 60000;
					  break;
					case SET_H :
					  et += 3600000;
					  break;
					default :
					  break;
				}
				break;
			case Button_Minus :
				switch (setmode) {
					case SET_S :
					  if (et>=1000) {
						et -= 1000;
					  }
					  break;
					case SET_M :
					  if (et>=60000) {
						et -= 60000;
					  }
					  break;
					case SET_H :
					  if (et>=3600000) {
						et -= 3600000;
					  }
					  break;
					default :
					  break;
				}
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_tt, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}
    ett = et; // save reset time
		displayTime(TFT_ORANGE, TFT_BLACK);
    mButton = No_Button;
	}
}

uint8_t TT_Running_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered TT_Running_Handler\n");
#endif
  et_last = et;
	erase_set_buttons();
	erase_clear_button();
	tft->drawRect(0, PROGBAR_Y, 239, PROGBAR_HEIGHT, TFT_DARKGREEN); // border progress bar
//  tft->fillRect(2, PROGBAR_Y+2, 235, PROGBAR_HEIGHT-4, TFT_DARKGREEN); // filled progress bar
  progbar = (et*235)/ett;
  tft->fillRect(2, PROGBAR_Y+2, progbar, PROGBAR_HEIGHT-4, TFT_DARKGREEN); // filled progress bar

	// main
	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_RunStop :
				return TT_paused_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_tt, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}
   
    clock_pointer++;
    if (clock_pointer>=8) clock_pointer=0;
    drawClockPointer();

    et = et_last - (millis() - start_time); // count down
    if (et>ett) et=0; // overflow?

    // prograss bar
    progbar = (et*235)/ett;
    //Serial.printf("progbar = %d, et = %d, ett = %d\n", progbar, et, ett);
    tft->fillRect(progbar+2, PROGBAR_Y+2, 235-progbar, PROGBAR_HEIGHT-4, TFT_BLACK);

		displayTime(TFT_ORANGE, TFT_BLACK);

    if (et<=0) { // time is up
			return TT_end_State;
		}
    mButton = No_Button;
	}
}

uint8_t TT_Paused_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered TT_Paused_Handler\n");
#endif
	et_last = et;
	flash_menu_item(3, swatch_menu, 1, 2, true, 3, 1, NULL, false, 0); // button clear

	// main
	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_Mode :
				return SW_cleared_State;
				break;
      case Button_Clear :
        return TT_cleared_State;
        break;
			case Button_RunStop :
        start_time = millis();
				return TT_running_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_tt, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}
		displayTime(TFT_ORANGE, TFT_BLACK);
    mButton = No_Button;
	}
}

uint8_t TT_End_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered TT_End_Handler\n");
#endif
	flash_menu_item(3, swatch_menu, 1, 2, true, 3, 1, NULL, true, 0); // button clear

	et=0;
	tft->fillCircle(120, 65, 18, CLOCK_FACE);
	clock_pointer=0;
	drawClockPointer();
	ettr_s = millis();
  start_time = ettr_s; // measure over time
	
	tft->drawRect(0, PROGBAR_Y, 239, PROGBAR_HEIGHT, TFT_RED); // border progress bar

  if (beepmode_tt==VIP_BEEP) {
	quickBuzz();
  }
  if (beepmode_tt>SILENT) {
	beep(9);
	beep(10);
	beep(11);
  }

	// main
	// event monitoring
	mButton = No_Button;
	while (mButton == No_Button) {
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
		switch (mButton) {  // only allow relevant buttons
			case Button_Mode :
				return SW_cleared_State;
				break;
			case Button_Clear :
				return TT_cleared_State;
				break;
			case Button_Sound :
				update_SOUND_button( &beepmode_tt, true );
				break;
			case Button_Exit :
				return Exit_state;
				break;
			default :
				mButton = No_Button;
				break;
		}

		if (beepmode_tt>SILENT) {
			// wait for clear and beep every minute
			ettr = millis() - ettr_s;
			// progress bar
			tft->fillRect(2, PROGBAR_Y+2, (ettr/255), PROGBAR_HEIGHT-4, TFT_RED);
			if (ettr > (60 * 1000)) {
			  tft->fillRect(2, PROGBAR_Y+2, 235, PROGBAR_HEIGHT-4, TFT_BLACK); // erase inner progress bar
			  ettr_s = millis();
			  if (beepmode_tt==VIP_BEEP) {
				quickBuzz();
			  }
			  beep(12);
			  beep(12);
			}
		}

    ett_ovr = ett + millis() - start_time; // count up overtime
    if ((ett_ovr-ett)>(30*1000)) {// show over time from 30s after end
      et = ett_ovr;
    }
		
		if ((millis() % 1000) <500 ) { // blink colors
			displayTime(TFT_RED, TFT_BLACK);
//      tft->setSwapBytes(true);
      tft->pushImage(20, 0, 32, 32, tea_bag);
		}
		else {
			displayTime(TFT_ORANGE, TFT_BLACK);
//      tft->setSwapBytes(true);
      tft->pushImage(20, 0, 32, 32, tea_cup);
		}
    mButton = No_Button;
	}
}

uint8_t Exit_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered Exit_Handler\n");
#endif

	tft->fillScreen(TFT_BLACK);
    return Exit_state;
}

uint8_t Error_Handler(void) {
	// entry
#if DBGSW
	Serial.printf("entered Error_Handler\n");
#endif

	tft->fillScreen(TFT_RED);
	// write text ...
    tft->setTextColor(TFT_BLACK, TFT_RED);
    tft->setTextSize(1);
    sprintf(buff, "Error:");
    tft->drawString(buff, 10, 50, 4);
    sprintf(buff, "should not be here...");
    tft->drawString(buff, 10, 100, 4);


	mButton = No_Button;
	while (mButton == No_Button) { // wait for any button
		mButton =  poll_swipe_or_menu_press(12);	// poll for touch, returns 0-15
#if DBGSW
    if (mButton != No_Button ) Serial.printf("mButton = %d\n", mButton);
#endif
	}

//  delay(1000);
    
    return Exit_state;
}

// the state machine
void appStopWatch(void) {
//  uint8_t eNextState = Init_State;
  eNextState = Init_State;
  boolean exit_event=false;
  while(!exit_event) {
#if DBGSW
  Serial.printf("State change eNextState = %d\n", eNextState);
#endif
    switch(eNextState) {
      case Init_State:
				eNextState = Init_Handler();
				break;
			case SW_cleared_State:
				eNextState = SW_Cleared_Handler();
				break;
			case SW_running_State:
				eNextState = SW_Running_Handler();
				break;
			case SW_paused_State:
				eNextState = SW_Paused_Handler();
				break;
			case TT_cleared_State:
				eNextState = TT_Cleared_Handler();
				break;
			case TT_running_State:
				eNextState = TT_Running_Handler();
				break;
			case TT_paused_State:
				eNextState = TT_Paused_Handler();
				break;
//			case TT_SetTime_State:
//				eNextState = TT_SetTime_Handler();
//				break;
			case TT_end_State:
				eNextState = TT_End_Handler();
				break;
			case Exit_state:
				eNextState = Exit_Handler();
				exit_event=true;
				break;
			case Error_state:
				eNextState = Error_Handler();
				exit_event=true;
				break;
			default:
				eNextState = Error_Handler();
				exit_event=true;
				break;
    }
    my_idle();
  }
}
